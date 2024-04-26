#include "duckdb.hpp"

#include "sqlite_db.hpp"
#include "sqlite_stmt.hpp"
#include "pbix_reader.hpp"
#include "pbix_scanner.hpp"

#include <stdint.h>
#include "duckdb/parser/parser.hpp"
#include "duckdb/parser/expression/cast_expression.hpp"
#include "duckdb/common/types/date.hpp"
#include "duckdb/common/types/timestamp.hpp"
#include "duckdb/storage/table/row_group.hpp"
#include "duckdb/main/client_context.hpp"
#include "duckdb/main/config.hpp"
#include "duckdb/storage/storage_extension.hpp"

#include "abf_parser.h"
#include "backup_log.h"

#include <cmath>
#include <algorithm>

namespace duckdb
{

	struct PbixLocalState : public LocalTableFunctionState
	{
		SQLiteDB *db;
		SQLiteDB owned_db;
		SQLiteStatement stmt;
		bool done = false;
		vector<column_t> column_ids;

		~PbixLocalState()
		{
		}
	};

	struct PbixGlobalState : public GlobalTableFunctionState
	{
		PbixGlobalState(idx_t max_threads) : max_threads(max_threads)
		{
		}

		mutex lock;
		idx_t position = 0;
		idx_t max_threads;

		idx_t MaxThreads() const override
		{
			return max_threads;
		}
	};

	static void ExtractDB(ClientContext &context, const string &path, int trailing_chunks, SQLiteDB &db, std::vector<VertipaqFile> &vertipaqFiles, bool &dm_error)
	{

		SQLiteOpenOptions options;
		auto dataModel = AbfParser::get_sqlite(context, path, trailing_chunks);
		db = SQLiteDB::OpenFromBuffer(options, dataModel.metadata_db);
		vertipaqFiles = dataModel.vertipaq_files;
		dm_error = dataModel.error_code;
	}

	static unique_ptr<FunctionData> PbixBind(ClientContext &context, TableFunctionBindInput &input,
											 vector<LogicalType> &return_types, vector<string> &names)
	{

		auto result = make_uniq<PbixBindData>();
		result->file_name = input.inputs[0].GetValue<string>();
		result->table_name = input.inputs[1].GetValue<string>();
		result->trailing_chunks = 15; // Empirically proven to be a good value
		Value pbix_magic_number;

		if (context.TryGetCurrentSetting("pbix_magic_number", pbix_magic_number))
		{
			result->trailing_chunks = IntegerValue::Get(pbix_magic_number);
		}

		SQLiteDB db;
		SQLiteOpenOptions options;
		options.access_mode = AccessMode::READ_ONLY;
		ExtractDB(context, result->file_name, result->trailing_chunks, db, result->vertipaq_files, result->data_model_error);

		ColumnList columns;
		db.GetMetaTableInfo(result->table_name, columns);
		for (auto &column : columns.Logical())
		{
			names.push_back(column.GetName());
			return_types.push_back(column.GetType());
		}

		if (names.empty())
		{
			throw std::runtime_error("no columns for table " + result->table_name);
		}

		if (!db.GetMaxRowId(result->table_name, result->max_rowid))
		{
			result->max_rowid = idx_t(-1);
			result->rows_per_group = idx_t(-1);
		}

		result->names = names;
		result->types = return_types;

		return std::move(result);
	}

	static void PbixInitInternal(ClientContext &context, const PbixBindData &bind_data, PbixLocalState &local_state,
								 idx_t rowid_min, idx_t rowid_max)
	{
		D_ASSERT(rowid_min <= rowid_max);

		local_state.done = false;
		// we may have leftover statements or connections from a previous call to this
		// function
		local_state.stmt.Close();
		if (!local_state.db)
		{
			SQLiteOpenOptions options;
			options.access_mode = AccessMode::READ_ONLY;
			int trailing_chunks = 15;
			Value pbix_magic_number;

			if (context.TryGetCurrentSetting("pbix_magic_number", pbix_magic_number))
			{
				trailing_chunks = IntegerValue::Get(pbix_magic_number);
			}

			std::vector<VertipaqFile> vertipaq_files;
			bool error_code;
			ExtractDB(context, bind_data.file_name.c_str(), trailing_chunks, local_state.owned_db, vertipaq_files, error_code);
			local_state.db = &local_state.owned_db;
		}

		auto col_names = StringUtil::Join(
			local_state.column_ids.data(), local_state.column_ids.size(), ", ", [&](const idx_t column_id)
			{ return column_id == (column_t)-1 ? "ROWID"
											   : '"' + SQLiteUtils::SanitizeIdentifier(bind_data.names[column_id]) + '"'; });

		auto sql =
			StringUtil::Format("SELECT c.ExplicitName,cs.StoragePosition,sfd.FileName,sfi.FileName,c.ExplicitDataType,ds.BaseId, ds.Magnitude FROM COLUMN c JOIN [Table] t ON c.TableId = t.ID JOIN ColumnStorage cs ON c.ColumnStorageID = cs.ID LEFT JOIN DictionaryStorage ds ON ds.ID = cs.DictionaryStorageID LEFT JOIN StorageFile sfd ON sfd.ID = ds.StorageFileID JOIN ColumnPartitionStorage cps ON cps.ColumnStorageID = cs.ID JOIN StorageFile sfi ON sfi.ID = cps.StorageFileID WHERE c.Type = 1 AND t.Name='%s'", SQLiteUtils::SanitizeIdentifier(bind_data.table_name));
		if (bind_data.rows_per_group != idx_t(-1))
		{
			// we are scanning a subset of the rows - generate a WHERE clause based on
			// the rowid
			auto where_clause = StringUtil::Format(" AND ROWID BETWEEN %d AND %d", rowid_min, rowid_max);
			sql += where_clause;
		}
		else
		{
			// we are scanning the entire table - no need for a WHERE clause
			D_ASSERT(rowid_min == 0);
		}
		local_state.stmt = local_state.db->Prepare(sql.c_str());
	}

	static unique_ptr<NodeStatistics> PbixCardinality(ClientContext &context, const FunctionData *bind_data_p)
	{
		D_ASSERT(bind_data_p);
		auto &bind_data = bind_data_p->Cast<PbixBindData>();
		return make_uniq<NodeStatistics>(bind_data.max_rowid);
	}

	static idx_t PbixMaxThreads(ClientContext &context, const FunctionData *bind_data_p)
	{
		D_ASSERT(bind_data_p);
		auto &bind_data = bind_data_p->Cast<PbixBindData>();
		if (bind_data.global_db)
		{
			return 1;
		}
		return bind_data.max_rowid / bind_data.rows_per_group;
	}

	static bool PbixParallelStateNext(ClientContext &context, const PbixBindData &bind_data, PbixLocalState &lstate,
									  PbixGlobalState &gstate)
	{
		lock_guard<mutex> parallel_lock(gstate.lock);
		if (gstate.position < bind_data.max_rowid)
		{
			auto start = gstate.position;
			auto end = start + bind_data.rows_per_group - 1;
			PbixInitInternal(context, bind_data, lstate, start, end);
			gstate.position = end + 1;
			return true;
		}
		return false;
	}

	static unique_ptr<LocalTableFunctionState>
	PbixInitLocalState(ExecutionContext &context, TableFunctionInitInput &input, GlobalTableFunctionState *global_state)
	{
		auto &bind_data = input.bind_data->Cast<PbixBindData>();
		auto &gstate = global_state->Cast<PbixGlobalState>();
		auto result = make_uniq<PbixLocalState>();
		result->column_ids = input.column_ids;
		result->db = bind_data.global_db;
		if (!PbixParallelStateNext(context.client, bind_data, *result, gstate))
		{
			result->done = true;
		}
		return std::move(result);
	}

	static unique_ptr<GlobalTableFunctionState> PbixInitGlobalState(ClientContext &context,
																	TableFunctionInitInput &input)
	{
		auto result = make_uniq<PbixGlobalState>(PbixMaxThreads(context, input.bind_data.get()));
		result->position = 0;
		return std::move(result);
	}

	static void PbixRead(ClientContext &context, TableFunctionInput &data, DataChunk &output)
	{
		auto &state = data.local_state->Cast<PbixLocalState>();
		auto &gstate = data.global_state->Cast<PbixGlobalState>();
		auto &bind_data = data.bind_data->Cast<PbixBindData>();

		VertipaqDecoder vdecoder(context, bind_data.file_name, bind_data.data_model_error);
		VertipaqData columnData;
		VertipaqFiles vertipaq_files;
		// put the vector from bind_data into the vertipaq_files map
		for (auto &vfile : bind_data.vertipaq_files)
		{
			vertipaq_files[vfile.FileName] = vfile;
		}


		// while (output.size() == 0)
		// {

			idx_t out_idx = 0;
			while (true)
			{
				if (state.done)	return;
				auto cardinality = out_idx / output.ColumnCount();

				if (cardinality >= STANDARD_VECTOR_SIZE) {
					output.SetCardinality(STANDARD_VECTOR_SIZE);
					return; // Manage chunking by limiting to the standard size
				}
				
				auto &stmt = state.stmt;
				auto has_more = stmt.Step();

				if (!has_more)
				{
					state.done = true;
					output.SetCardinality(cardinality);
					break; // Finished processing all rows
				}

				// Handle NULL values by checking before constructing std::string
				const char *dictionary = reinterpret_cast<const char *>(sqlite3_value_text(stmt.GetValue<sqlite3_value *>(2)));
				const char *idf = reinterpret_cast<const char *>(sqlite3_value_text(stmt.GetValue<sqlite3_value *>(3)));

				VertipaqDetails details = {
					sqlite3_value_int64(stmt.GetValue<sqlite3_value *>(1)),
					dictionary ? std::string(dictionary) : "", // Use empty string if NULL
					idf ? std::string(idf) : "",			   // Use empty string if NULL
					sqlite3_value_int64(stmt.GetValue<sqlite3_value *>(4)),
					sqlite3_value_int64(stmt.GetValue<sqlite3_value *>(5)),
					sqlite3_value_double(stmt.GetValue<sqlite3_value *>(6)),
				};
		

				idx_t col_idx = details.StoragePosition - 1;
				auto &out_vec = output.data[col_idx];

				if (dictionary)
				{
					// Process string data
					auto result_vec = vdecoder.processVertipaqStr(details, vertipaq_files);
					out_idx +=std::min(int(result_vec.size()), STANDARD_VECTOR_SIZE);

					for(idx_t i = 0; i < std::min(int(result_vec.size()), STANDARD_VECTOR_SIZE); i++)
					{
						output.SetValue(col_idx,i,result_vec[i]);
					}
				}
				else
				{
					// Process integer data
					auto result_vec = vdecoder.processVertipaqInt(details, vertipaq_files);
					out_idx += std::min(int(result_vec.size()), STANDARD_VECTOR_SIZE);
					for(idx_t i = 0; i < std::min(int(result_vec.size()), STANDARD_VECTOR_SIZE); i++)
					{
						output.SetValue(col_idx,i,duckdb::Value::BIGINT(result_vec[i]));
					}
				}
			}
		
	}

	static string SqliteToString(const FunctionData *bind_data_p)
	{
		D_ASSERT(bind_data_p);
		auto &bind_data = bind_data_p->Cast<PbixBindData>();
		return StringUtil::Format("%s:%s", bind_data.file_name, bind_data.table_name);
	}

	PbixReadFunction::PbixReadFunction()
		: TableFunction("pbix_read", {LogicalType::VARCHAR, LogicalType::VARCHAR}, PbixRead, PbixBind,
						PbixInitGlobalState, PbixInitLocalState)
	{
		cardinality = PbixCardinality;
		to_string = SqliteToString;
		projection_pushdown = true;
	}

} // namespace duckdb
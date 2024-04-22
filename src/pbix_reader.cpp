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

namespace duckdb {

struct PbixLocalState : public LocalTableFunctionState {
	SQLiteDB *db;
	SQLiteDB owned_db;
	SQLiteStatement stmt;
	bool done = false;
	vector<column_t> column_ids;

	~PbixLocalState() {
	}
};

struct PbixGlobalState : public GlobalTableFunctionState {
	PbixGlobalState(idx_t max_threads) : max_threads(max_threads) {
	}

	mutex lock;
	idx_t position = 0;
	idx_t max_threads;

	idx_t MaxThreads() const override {
		return max_threads;
	}
};


static SQLiteDB ExtractDB(ClientContext &context, const string &path, int trailing_chunks) {

	SQLiteOpenOptions options;
	auto [sqliteBuffer,vertipaqFiles] = AbfParser::get_sqlite(context, path, trailing_chunks);
	// std::cout << "ExtractDB: " << sqliteBuffer.size() << std::endl;
	// std::cout << "backup_log: " << backup_log.size() << std::endl;
	// //cout every backup log file
	// for (auto &log : backup_log) {
	// 	std::cout << "FileName: " << log.FileName << std::endl;
	// 	std::cout << "Path: " << log.Path << std::endl;
	// 	std::cout << "Size: " << log.Size << std::endl;
	// 	std::cout << "StoragePath: " << log.StoragePath << std::endl;
	// 	std::cout << "m_cbOffsetHeader: " << log.m_cbOffsetHeader << std::endl;
	// 	std::cout << "==========================================" << std::endl;
	// }
	return SQLiteDB::OpenFromBuffer(options, sqliteBuffer);

}

static unique_ptr<FunctionData> PbixBind(ClientContext &context, TableFunctionBindInput &input,
                                           vector<LogicalType> &return_types, vector<string> &names) {

	auto result = make_uniq<PbixBindData>();
	result->file_name = input.inputs[0].GetValue<string>();
	result->table_name = input.inputs[1].GetValue<string>();
	result->trailing_chunks = 15; //Empirically proven to be a good value
	Value pbix_magic_number;

	if (context.TryGetCurrentSetting("pbix_magic_number", pbix_magic_number)) {
			result->trailing_chunks = IntegerValue::Get(pbix_magic_number);
	}

	SQLiteDB db;
	SQLiteOpenOptions options;
	options.access_mode = AccessMode::READ_ONLY;
	db = ExtractDB(context, result->file_name, result->trailing_chunks);

	ColumnList columns;
	db.GetMetaTableInfo(result->table_name, columns);
	for (auto &column : columns.Logical()) {
		names.push_back(column.GetName());
		return_types.push_back(column.GetType());
	}

	if (names.empty()) {
		throw std::runtime_error("no columns for table " + result->table_name);
	}

	if (!db.GetMaxRowId(result->table_name, result->max_rowid)) {
		result->max_rowid = idx_t(-1);
		result->rows_per_group = idx_t(-1);
	}

	result->names = names;
	result->types = return_types;

	return std::move(result);
}

static void PbixInitInternal(ClientContext &context, const PbixBindData &bind_data, PbixLocalState &local_state,
                               idx_t rowid_min, idx_t rowid_max) {
	D_ASSERT(rowid_min <= rowid_max);

	local_state.done = false;
	// we may have leftover statements or connections from a previous call to this
	// function
	local_state.stmt.Close();
	if (!local_state.db) {
		SQLiteOpenOptions options;
		options.access_mode = AccessMode::READ_ONLY;
		int trailing_chunks = 15;
		Value pbix_magic_number;

		if (context.TryGetCurrentSetting("pbix_magic_number", pbix_magic_number)) {
				trailing_chunks = IntegerValue::Get(pbix_magic_number);
		}

		local_state.owned_db = ExtractDB(context, bind_data.file_name.c_str(),trailing_chunks);
		local_state.db = &local_state.owned_db;
	}

	auto col_names = StringUtil::Join(
	    local_state.column_ids.data(), local_state.column_ids.size(), ", ", [&](const idx_t column_id) {
		    return column_id == (column_t)-1 ? "ROWID"
		                                     : '"' + SQLiteUtils::SanitizeIdentifier(bind_data.names[column_id]) + '"';
	    });

	auto sql =
	    StringUtil::Format("SELECT t.Name AS TableName, c.ExplicitName AS ColumnName, cs.StoragePosition, sfd.FileName AS Dictionary, sfh.FileName AS HIDX, sfi.FileName AS IDF, cs.Statistics_DistinctStates as Cardinality,ds.DataType,ds.BaseId,ds.Magnitude FROM Column c JOIN [Table] t ON c.TableId = t.ID JOIN ColumnStorage cs ON c.ColumnStorageID = cs.ID JOIN AttributeHierarchy ah ON ah.ColumnID = c.ID JOIN AttributeHierarchyStorage ahs ON ah.AttributeHierarchyStorageID = ahs.ID LEFT JOIN StorageFile sfh ON sfh.ID = ahs.StorageFileID LEFT JOIN DictionaryStorage ds ON ds.ID = cs.DictionaryStorageID LEFT JOIN StorageFile sfd ON sfd.ID = ds.StorageFileID JOIN ColumnPartitionStorage cps ON cps.ColumnStorageID = cs.ID JOIN StorageFile sfi ON sfi.ID = cps.StorageFileID WHERE c.Type = 1 AND t.Name= '%s'", SQLiteUtils::SanitizeIdentifier(bind_data.table_name));
	if (bind_data.rows_per_group != idx_t(-1)) {
		// we are scanning a subset of the rows - generate a WHERE clause based on
		// the rowid
		auto where_clause = StringUtil::Format(" AND ROWID BETWEEN %d AND %d", rowid_min, rowid_max);
		sql += where_clause;
	} else {
		// we are scanning the entire table - no need for a WHERE clause
		D_ASSERT(rowid_min == 0);
	}
	local_state.stmt = local_state.db->Prepare(sql.c_str());
}

static unique_ptr<NodeStatistics> PbixCardinality(ClientContext &context, const FunctionData *bind_data_p) {
	D_ASSERT(bind_data_p);
	auto &bind_data = bind_data_p->Cast<PbixBindData>();
	return make_uniq<NodeStatistics>(bind_data.max_rowid);
}

static idx_t PbixMaxThreads(ClientContext &context, const FunctionData *bind_data_p) {
	D_ASSERT(bind_data_p);
	auto &bind_data = bind_data_p->Cast<PbixBindData>();
	if (bind_data.global_db) {
		return 1;
	}
	return bind_data.max_rowid / bind_data.rows_per_group;
}

static bool PbixParallelStateNext(ClientContext &context, const PbixBindData &bind_data, PbixLocalState &lstate,
                                    PbixGlobalState &gstate) {
	lock_guard<mutex> parallel_lock(gstate.lock);
	if (gstate.position < bind_data.max_rowid) {
		auto start = gstate.position;
		auto end = start + bind_data.rows_per_group - 1;
		PbixInitInternal(context, bind_data, lstate, start, end);
		gstate.position = end + 1;
		return true;
	}
	return false;
}

static unique_ptr<LocalTableFunctionState>
PbixInitLocalState(ExecutionContext &context, TableFunctionInitInput &input, GlobalTableFunctionState *global_state) {
	auto &bind_data = input.bind_data->Cast<PbixBindData>();
	auto &gstate = global_state->Cast<PbixGlobalState>();
	auto result = make_uniq<PbixLocalState>();
	result->column_ids = input.column_ids;
	result->db = bind_data.global_db;
	if (!PbixParallelStateNext(context.client, bind_data, *result, gstate)) {
		result->done = true;
	}
	return std::move(result);
}

static unique_ptr<GlobalTableFunctionState> PbixInitGlobalState(ClientContext &context,
                                                                  TableFunctionInitInput &input) {
	auto result = make_uniq<PbixGlobalState>(PbixMaxThreads(context, input.bind_data.get()));
	result->position = 0;
	return std::move(result);
}

static void PbixRead(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
	auto &state = data.local_state->Cast<PbixLocalState>();
	auto &gstate = data.global_state->Cast<PbixGlobalState>();
	auto &bind_data = data.bind_data->Cast<PbixBindData>();

	while (output.size() == 0) {
		if (state.done) {
			if (!PbixParallelStateNext(context, bind_data, state, gstate)) {
				return;
			}
		}

		idx_t out_idx = 0;
		while (true) {
			if (out_idx == STANDARD_VECTOR_SIZE) {
				output.SetCardinality(out_idx);
				return;
			}
			auto &stmt = state.stmt;
			auto has_more = stmt.Step();
			if (!has_more) {
				state.done = true;
				output.SetCardinality(out_idx);
				break;
			}
			for (idx_t col_idx = 0; col_idx < output.ColumnCount(); col_idx++) {
				auto &out_vec = output.data[col_idx];
				auto sqlite_column_type = stmt.GetType(col_idx);
				if (sqlite_column_type == SQLITE_NULL) {
					auto &mask = FlatVector::Validity(out_vec);
					mask.Set(out_idx, false);
					continue;
				}

				auto val = stmt.GetValue<sqlite3_value *>(col_idx);
				switch (out_vec.GetType().id()) {
				case LogicalTypeId::BIGINT:
					stmt.CheckTypeMatches(bind_data, val, sqlite_column_type, SQLITE_INTEGER, col_idx);
					FlatVector::GetData<int64_t>(out_vec)[out_idx] = sqlite3_value_int64(val);
					break;
				case LogicalTypeId::DOUBLE:
					stmt.CheckTypeIsFloatOrInteger(val, sqlite_column_type, col_idx);
					FlatVector::GetData<double>(out_vec)[out_idx] = sqlite3_value_double(val);
					break;
				case LogicalTypeId::VARCHAR:
					stmt.CheckTypeMatches(bind_data, val, sqlite_column_type, SQLITE_TEXT, col_idx);
					FlatVector::GetData<string_t>(out_vec)[out_idx] = StringVector::AddString(
					    out_vec, (const char *)sqlite3_value_text(val), sqlite3_value_bytes(val));
					break;
				case LogicalTypeId::DATE:
					stmt.CheckTypeMatches(bind_data, val, sqlite_column_type, SQLITE_TEXT, col_idx);
					FlatVector::GetData<date_t>(out_vec)[out_idx] =
					    Date::FromCString((const char *)sqlite3_value_text(val), sqlite3_value_bytes(val));
					break;
				case LogicalTypeId::TIMESTAMP:
					stmt.CheckTypeMatches(bind_data, val, sqlite_column_type, SQLITE_TEXT, col_idx);
					FlatVector::GetData<timestamp_t>(out_vec)[out_idx] =
					    Timestamp::FromCString((const char *)sqlite3_value_text(val), sqlite3_value_bytes(val));
					break;
				case LogicalTypeId::BLOB:
					FlatVector::GetData<string_t>(out_vec)[out_idx] = StringVector::AddStringOrBlob(
					    out_vec, (const char *)sqlite3_value_blob(val), sqlite3_value_bytes(val));
					break;
				default:
					throw std::runtime_error(out_vec.GetType().ToString());
				}
			}
			out_idx++;
		}
	}
}


static string SqliteToString(const FunctionData *bind_data_p) {
	D_ASSERT(bind_data_p);
	auto &bind_data = bind_data_p->Cast<PbixBindData>();
	return StringUtil::Format("%s:%s", bind_data.file_name, bind_data.table_name);
}

PbixReadFunction::PbixReadFunction()
	: TableFunction("pbix_read", {LogicalType::VARCHAR, LogicalType::VARCHAR}, PbixRead, PbixBind,
					PbixInitGlobalState, PbixInitLocalState) {
	cardinality = PbixCardinality;
	to_string = SqliteToString;
	projection_pushdown = true;
					
}

} // namespace duckdb
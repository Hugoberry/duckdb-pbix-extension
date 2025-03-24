#include "duckdb.hpp"

#include "sqlite_db.hpp"
#include "sqlite_stmt.hpp"
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
#include "duckdb/common/operator/cast_operators.hpp"

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
	//! The amount of rows we scanned as part of this row group
	idx_t scan_count = 1;

	~PbixLocalState() {
	}
};

struct PbixGlobalState : public GlobalTableFunctionState {
	explicit PbixGlobalState(idx_t max_threads) : max_threads(max_threads) {
	}

	mutex lock;
	idx_t position = 0;
	idx_t max_threads;
	idx_t rows_per_group = 122880;

	idx_t MaxThreads() const override {
		return max_threads;
	}
};

static SQLiteDB ExtractDB(ClientContext &context, const string &path, int trailing_chunks) {
	SQLiteOpenOptions options;
	auto dataModel = AbfParser::get_sqlite(context, path, trailing_chunks);
	return SQLiteDB::OpenFromBuffer(options, dataModel.metadata_db);
}

static unique_ptr<FunctionData> PbixBind(ClientContext &context, TableFunctionBindInput &input,
                                           vector<LogicalType> &return_types, vector<string> &names) {
	auto result = make_uniq<PbixBindData>();
	result->file_name = input.inputs[0].GetValue<string>();
	result->table_name = input.inputs[1].GetValue<string>();
	result->trailing_chunks = 15; // Empirically proven to be a good value
	Value pbix_magic_number;

	if (context.TryGetCurrentSetting("pbix_magic_number", pbix_magic_number)) {
		result->trailing_chunks = IntegerValue::Get(pbix_magic_number);
	}

	SQLiteDB db;
	SQLiteOpenOptions options;
	options.access_mode = AccessMode::READ_ONLY;
	db = ExtractDB(context, result->file_name, result->trailing_chunks);
	
	ColumnList columns;
	vector<unique_ptr<Constraint>> constraints;

	db.GetTableInfo(result->table_name, columns/*, constraints*/);
	for (auto &column : columns.Logical()) {
		names.push_back(column.GetName());
		return_types.push_back(column.GetType());
	}

	if (names.empty()) {
		throw std::runtime_error("no columns for table " + result->table_name);
	}

	// Get row ID information for parallelization
	if (!db.GetRowIdInfo(result->table_name, result->row_id_info)) {
		result->rows_per_group = optional_idx();
	} else {
		// Use the default rows_per_group if row_id_info is valid
		result->rows_per_group = optional_idx(122880); // Default from your original code
	}

	result->names = names;
	result->types = return_types;

	return std::move(result);
}

static void PbixInitInternal(ClientContext &context, const PbixBindData &bind_data, PbixLocalState &local_state,
                               idx_t rowid_min, idx_t rowid_max) {
	D_ASSERT(rowid_min <= rowid_max);

	local_state.done = false;
	// we may have leftover statements or connections from a previous call to this function
	local_state.stmt.Close();
	if (!local_state.db) {
		SQLiteOpenOptions options;
		options.access_mode = AccessMode::READ_ONLY;
		int trailing_chunks = bind_data.trailing_chunks;
		Value pbix_magic_number;

		if (context.TryGetCurrentSetting("pbix_magic_number", pbix_magic_number)) {
			trailing_chunks = IntegerValue::Get(pbix_magic_number);
		}

		local_state.owned_db = ExtractDB(context, bind_data.file_name.c_str(), trailing_chunks);
		local_state.db = &local_state.owned_db;
	}

	string sql;
	if (bind_data.sql.empty()) {
		auto col_names = StringUtil::Join(
			local_state.column_ids.data(), local_state.column_ids.size(), ", ", [&](const idx_t column_id) {
				return column_id == (column_t)-1 ? "ROWID"
												 : '"' + SQLiteUtils::SanitizeIdentifier(bind_data.names[column_id]) + '"';
			});

		sql = StringUtil::Format("SELECT %s FROM \"%s\"", col_names, SQLiteUtils::SanitizeIdentifier(bind_data.table_name));
		if (bind_data.rows_per_group.IsValid()) {
			// we are scanning a subset of the rows - generate a WHERE clause based on the rowid
			auto where_clause = StringUtil::Format(" WHERE ROWID BETWEEN %d AND %d", rowid_min, rowid_max);
			sql += where_clause;
		} else {
			// we are scanning the entire table - no need for a WHERE clause
			D_ASSERT(rowid_min == 0);
		}
	} else {
		sql = bind_data.sql;
	}
	local_state.stmt = local_state.db->Prepare(sql.c_str());
}

static unique_ptr<NodeStatistics> PbixCardinality(ClientContext &context, const FunctionData *bind_data_p) {
	D_ASSERT(bind_data_p);
	auto &bind_data = bind_data_p->Cast<PbixBindData>();
	if (!bind_data.row_id_info.max_rowid.IsValid()) {
		return nullptr;
	}
	auto row_count = bind_data.row_id_info.max_rowid.GetIndex() - bind_data.row_id_info.min_rowid.GetIndex();
	return make_uniq<NodeStatistics>(row_count);
}

static idx_t PbixMaxThreads(ClientContext &context, const FunctionData *bind_data_p) {
	D_ASSERT(bind_data_p);
	auto &bind_data = bind_data_p->Cast<PbixBindData>();
	if (bind_data.global_db) {
		return 1;
	}
	if (!bind_data.row_id_info.max_rowid.IsValid()) {
		return 1;
	}
	auto row_count = bind_data.row_id_info.max_rowid.GetIndex() - bind_data.row_id_info.min_rowid.GetIndex();
	return row_count / bind_data.rows_per_group.GetIndex();
}

static bool PbixParallelStateNext(ClientContext &context, const PbixBindData &bind_data, PbixLocalState &lstate,
                                    PbixGlobalState &gstate) {
	lock_guard<mutex> parallel_lock(gstate.lock);
	if (!bind_data.rows_per_group.IsValid()) {
		// not doing a parallel scan - scan everything at once
		if (gstate.position > 0) {
			// already scanned
			return false;
		}
		PbixInitInternal(context, bind_data, lstate, 0, 0);
		gstate.position = static_cast<idx_t>(-1);
		lstate.scan_count = 0;
		return true;
	}
	auto max_row_id = bind_data.row_id_info.max_rowid.GetIndex();
	if (gstate.position < max_row_id) {
		if (lstate.scan_count == 0 && gstate.rows_per_group < max_row_id) {
			// we scanned no rows in our previous slice - double the rows per group
			gstate.rows_per_group *= 2;
		}
		if (gstate.rows_per_group == 0) {
			throw InternalException("PbixParallelStateNext - gstate.rows_per_group not set");
		}
		auto start = gstate.position;
		auto end = MinValue<idx_t>(max_row_id, start + gstate.rows_per_group - 1);
		PbixInitInternal(context, bind_data, lstate, start, end);
		gstate.position = end + 1;
		lstate.scan_count = 0;
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
	auto &bind_data = input.bind_data->Cast<PbixBindData>();
	auto result = make_uniq<PbixGlobalState>(PbixMaxThreads(context, input.bind_data.get()));
	result->position = 0;
	if (bind_data.rows_per_group.IsValid()) {
		auto min_row_id = bind_data.row_id_info.min_rowid.GetIndex();
		if (min_row_id > 0) {
			result->position = min_row_id - 1;
		}
		result->rows_per_group = bind_data.rows_per_group.GetIndex();
	}
	return std::move(result);
}

static timestamp_t ConvertTimestampInteger(sqlite3_value *val) {
	return Timestamp::FromEpochSeconds(sqlite3_value_int64(val));
}

static timestamp_t ConvertTimestampFloat(sqlite3_value *val) {
	int64_t timestamp_micros = Cast::Operation<double, int64_t>(sqlite3_value_double(val) * 1000000.0);
	return Timestamp::FromEpochMicroSeconds(timestamp_micros);
}

static void PbixScan(ClientContext &context, TableFunctionInput &data, DataChunk &output) {
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
			state.scan_count++;
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
					if (sqlite_column_type == SQLITE_INTEGER) {
						// unix timestamp
						FlatVector::GetData<date_t>(out_vec)[out_idx] =
							Timestamp::GetDate(ConvertTimestampInteger(val));
					} else if (sqlite_column_type == SQLITE_FLOAT) {
						FlatVector::GetData<date_t>(out_vec)[out_idx] = Timestamp::GetDate(ConvertTimestampFloat(val));
					} else if (sqlite_column_type == SQLITE_TEXT) {
						FlatVector::GetData<date_t>(out_vec)[out_idx] =
							Date::FromCString((const char *)sqlite3_value_text(val), sqlite3_value_bytes(val));
					} else {
						throw NotImplementedException("Unimplemented SQLite type for column of type DATE");
					}
					break;
				case LogicalTypeId::TIMESTAMP:
					// SQLite does not have a timestamp type - but it has "conventions"
					if (sqlite_column_type == SQLITE_INTEGER) {
						// unix timestamp
						FlatVector::GetData<timestamp_t>(out_vec)[out_idx] = ConvertTimestampInteger(val);
					} else if (sqlite_column_type == SQLITE_FLOAT) {
						FlatVector::GetData<timestamp_t>(out_vec)[out_idx] = ConvertTimestampFloat(val);
					} else if (sqlite_column_type == SQLITE_TEXT) {
						// ISO-8601
						FlatVector::GetData<timestamp_t>(out_vec)[out_idx] =
							Timestamp::FromCString((const char *)sqlite3_value_text(val), sqlite3_value_bytes(val));
					} else {
						throw NotImplementedException("Unimplemented SQLite type for column of type TIMESTAMP");
					}
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

static InsertionOrderPreservingMap<string> PbixToString(TableFunctionToStringInput &input) {
	D_ASSERT(input.bind_data);
	InsertionOrderPreservingMap<string> result;
	auto &bind_data = input.bind_data->Cast<PbixBindData>();
	result["Table"] = bind_data.table_name;
	result["File"] = bind_data.file_name;
	return result;
}

BindInfo PbixMetaBindInfo(const optional_ptr<FunctionData> bind_data_p) {
	BindInfo info(ScanType::EXTERNAL);
	auto &bind_data = bind_data_p->Cast<PbixBindData>();
	info.table = bind_data.table;
	return info;
}

PbixScanFunction::PbixScanFunction()
    : TableFunction("pbix_meta", {LogicalType::VARCHAR, LogicalType::VARCHAR}, PbixScan, PbixBind,
                  PbixInitGlobalState, PbixInitLocalState) {
    cardinality = PbixCardinality;
    to_string = PbixToString;
    get_bind_info = PbixMetaBindInfo;
    projection_pushdown = true;
}

} // namespace duckdb
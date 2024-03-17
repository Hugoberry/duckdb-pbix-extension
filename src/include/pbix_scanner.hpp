#pragma once

#include "duckdb.hpp"

namespace duckdb {
class SQLiteDB;

struct PbixBindData : public TableFunctionData {
	string file_name;
	string table_name;

	vector<string> names;
	vector<LogicalType> types;

	idx_t max_rowid = 0;
	bool all_varchar = false;

	idx_t rows_per_group = 122880;
	SQLiteDB *global_db;
};

class PbixScanFunction : public TableFunction {
public:
	PbixScanFunction();
};


} 
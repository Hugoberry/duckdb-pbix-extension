#pragma once

#include "duckdb.hpp"

namespace duckdb {
class Pbix;

struct PbixBindData : public TableFunctionData {
	string file_name;
	string table_name;

	vector<string> names;
	vector<LogicalType> types;

	idx_t max_rowid = 0;
	bool all_varchar = false;

	idx_t rows_per_group = 122880;
	Pbix *global_db;
};

class PbixScanFunction : public TableFunction {
public:
	PbixScanFunction();
};

class PbixAttachFunction : public TableFunction {
public:
	PbixAttachFunction();
};

} 
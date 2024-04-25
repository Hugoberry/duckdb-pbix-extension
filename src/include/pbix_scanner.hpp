#pragma once

#include "duckdb.hpp"
#include "abf_parser.h"

namespace duckdb
{

	class SQLiteDB;

	struct PbixBindData : public TableFunctionData
	{
		string file_name;
		string table_name;
		std::vector<VertipaqFile> vertipaq_files;
		unique_ptr<FileHandle> file_handle;     
		bool data_model_error;
		int8_t trailing_chunks;

		vector<string> names;
		vector<LogicalType> types;

		idx_t max_rowid = 0;
		bool all_varchar = false;

		idx_t rows_per_group = 122880;
		SQLiteDB *global_db;
	};

	class PbixScanFunction : public TableFunction
	{
	public:
		PbixScanFunction();
	};

}
#pragma once

#include "duckdb.hpp"
#include "abf_parser.h"
#include "sqlite_utils.hpp"

namespace duckdb {

class SQLiteDB;

struct PbixBindData : public TableFunctionData {
    // File information
    string file_name;
    string table_name;
    string sql;
    
    // PBIX specific
    std::vector<VertipaqFile> vertipaq_files;
    unique_ptr<FileHandle> file_handle;     
    bool data_model_error = false;
    int trailing_chunks = 15;
    
    // Schema information
    vector<string> names;
    vector<LogicalType> types;
    
    // Parallelization info
    RowIdInfo row_id_info;
    optional_idx rows_per_group;
    
    // SQLite connection
    SQLiteDB *global_db = nullptr;
	bool all_varchar = false;
    
    // Catalog entries for binding
    SchemaCatalogEntry *schema = nullptr;
    TableCatalogEntry *table = nullptr;
};

class PbixScanFunction : public TableFunction {
public:
    PbixScanFunction();
};

} // namespace duckdb
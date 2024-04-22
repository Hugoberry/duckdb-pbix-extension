#ifndef DUCKDB_BUILD_LOADABLE_EXTENSION
#define DUCKDB_BUILD_LOADABLE_EXTENSION
#endif
#include "duckdb.hpp"

#include "pbix_scanner.hpp"
#include "pbix_reader.hpp"
#include "pbix_extension.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/main/extension_util.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"

using namespace duckdb;


extern "C"
{

    static void LoadInternal(DatabaseInstance &db)
    {
        PbixScanFunction pbix_fun;
        ExtensionUtil::RegisterFunction(db, pbix_fun);
        PbixReadFunction pbix_read_fun;
        ExtensionUtil::RegisterFunction(db, pbix_read_fun);
        auto &config = DBConfig::GetConfig(db);
	    config.AddExtensionOption("pbix_magic_number", "A magic number to accelerate file parsing", LogicalType::INTEGER);
    }

    void PbixExtension::Load(DuckDB &db)
    {
        LoadInternal(*db.instance);
    }

    DUCKDB_EXTENSION_API void pbix_init(duckdb::DatabaseInstance &db)
    {
        LoadInternal(db);
    }

    DUCKDB_EXTENSION_API const char *pbix_version()
    {
        return DuckDB::LibraryVersion();
    }
}
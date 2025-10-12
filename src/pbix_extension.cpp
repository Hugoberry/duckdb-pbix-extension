#ifndef DUCKDB_BUILD_LOADABLE_EXTENSION
#define DUCKDB_BUILD_LOADABLE_EXTENSION
#endif
#include "duckdb.hpp"

#include "pbix_scanner.hpp"
#include "pbix_reader.hpp"
#include "pbix_extension.hpp"
#include "vpax_builder.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"

using namespace duckdb;


extern "C"
{

    static void LoadInternal(ExtensionLoader &loader)
    {
        PbixScanFunction pbix_fun;
        loader.RegisterFunction(pbix_fun);
        PbixReadFunction pbix_read_fun;
        loader.RegisterFunction(pbix_read_fun);

        VpaxFunction::Register(loader);

        auto &db = loader.GetDatabaseInstance();
        auto &config = DBConfig::GetConfig(db);
	    config.AddExtensionOption("pbix_trailing_chunks_optimization", "Number of trailing chunks to read for optimized file parsing", LogicalType::INTEGER);
	    config.AddExtensionOption("pbix_ignore_errors", "Return empty VPAX structure instead of throwing exceptions on parse errors", LogicalType::BOOLEAN);
    }

    void PbixExtension::Load(ExtensionLoader &loader)
    {
        LoadInternal(loader);
    }

    DUCKDB_CPP_EXTENSION_ENTRY(pbix,loader)
    {
        LoadInternal(loader);
    }

}
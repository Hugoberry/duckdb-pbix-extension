#define DUCKDB_EXTENSION_MAIN

#include "pbix_extension.hpp"

#include "pbix_scanner.hpp"
#include "pbix_storage.hpp"

#include "AbfParser.h"
#include "Xpress9Wrapper.h"

#include "duckdb.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/main/extension_util.hpp"

#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>
#include <duckdb/parser/parsed_data/create_table_function_info.hpp>


namespace duckdb {

inline void PbixScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Pbix "+name.GetString()+" 🐥");;
        });
}

inline void PbixScannerScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    // Assuming the first argument is a file path
    auto &path_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
        path_vector, result, args.size(),
        [&](string_t path) {
            std::ifstream file(path.GetString());
            std::string content((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
            file.close();
            return StringVector::AddString(result, "Pbix " + content + " 🐥");
        });
}
static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto pbix_scalar_function = ScalarFunction("pbix", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PbixScalarFun);
    ExtensionUtil::RegisterFunction(instance, pbix_scalar_function);
    
    auto pbix_scanner_scalar_function = ScalarFunction("pbix_scanner", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PbixScannerScalarFun);
    ExtensionUtil::RegisterFunction(instance, pbix_scanner_scalar_function);
}

void PbixExtension::Load(DuckDB &db) {
    auto &db_instance = *db.instance;
    auto &fs = db.GetFileSystem();
    LoadInternal(db_instance);
}
std::string PbixExtension::Name() {
	return "pbix";
}

} // namespace duckdb

extern "C" {

DUCKDB_EXTENSION_API void pbix_init(duckdb::DatabaseInstance &db) {
    duckdb::DuckDB db_wrapper(db);
    db_wrapper.LoadExtension<duckdb::PbixExtension>();
}

DUCKDB_EXTENSION_API const char *pbix_version() {
	return duckdb::DuckDB::LibraryVersion();
}
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif

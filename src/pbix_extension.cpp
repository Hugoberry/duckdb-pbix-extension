#define DUCKDB_EXTENSION_MAIN

#include "pbix_extension.hpp"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>

// OpenSSL linked through vcpkg
#include <openssl/opensslv.h>

namespace duckdb {

inline void PbixScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Pbix "+name.GetString()+" 🐥");;
        });
}

inline void PbixOpenSSLVersionScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Pbix " + name.GetString() +
                                                     ", my linked OpenSSL version is " +
                                                     OPENSSL_VERSION_TEXT );;
        });
}

static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto pbix_scalar_function = ScalarFunction("pbix", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PbixScalarFun);
    ExtensionUtil::RegisterFunction(instance, pbix_scalar_function);

    // Register another scalar function
    auto pbix_openssl_version_scalar_function = ScalarFunction("pbix_openssl_version", {LogicalType::VARCHAR},
                                                LogicalType::VARCHAR, PbixOpenSSLVersionScalarFun);
    ExtensionUtil::RegisterFunction(instance, pbix_openssl_version_scalar_function);
}

void PbixExtension::Load(DuckDB &db) {
	LoadInternal(*db.instance);
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

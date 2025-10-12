#ifndef DUCKDB_BUILD_LOADABLE_EXTENSION
#define DUCKDB_BUILD_LOADABLE_EXTENSION
#endif
#include "duckdb.hpp"

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"

using namespace duckdb;


class PbixExtension{
public:
	static void Load(ExtensionLoader &loader);
	static std::string Name() {
		return "pbix";
	}
	std::string Version() const {
#ifdef EXT_VERSION_PBIX
	return EXT_VERSION_PBIX;
#else
	return "v0.5.1";
#endif
	}
};


#ifndef DUCKDB_BUILD_LOADABLE_EXTENSION
#define DUCKDB_BUILD_LOADABLE_EXTENSION
#endif
#include "duckdb.hpp"
#include <tinyxml2.h>

#include "duckdb/catalog/catalog.hpp"
#include "duckdb/parser/parsed_data/create_table_function_info.hpp"

using namespace duckdb;

class PbixExtension : public Extension {
public:
	std::string Name() override {
		return "pbix";
	}
	void Load(DuckDB &db) override;
};

extern "C" {
DUCKDB_EXTENSION_API void pbix_init(duckdb::DatabaseInstance &db);
DUCKDB_EXTENSION_API const char *pbix_version();

}
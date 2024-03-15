#define DUCKDB_EXTENSION_MAIN

#include "pbix_extension.hpp"
#include "AbfParser.h"
#include "Xpress9Wrapper.h"
#include "duckdb.hpp"
#include "duckdb/common/exception.hpp"
#include "duckdb/common/string_util.hpp"
#include "duckdb/function/scalar_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include <duckdb/parser/parsed_data/create_scalar_function_info.hpp>


namespace duckdb {

inline void PbixScalarFun(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    UnaryExecutor::Execute<string_t, string_t>(
	    name_vector, result, args.size(),
	    [&](string_t name) {
			return StringVector::AddString(result, "Pbix "+name.GetString()+" 🐥");;
        });
}

inline voide DecompressAndProcessPbix(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &name_vector = args.data[0];
    ifstream entryStream(name_vector, std::ios::binary);
    entryStream.seekg(102, std::ios::beg);
    XPress9Wrapper xpress9Wrapper;
    xpress9Wrapper.Initialize(false, true);
    vector<uint8_t> allDecompressedData;
    entryStream.seekg(0, ios::end);
    auto totalSize = entryStream.tellg();
    entryStream.seekg(102, ios::beg);
    while (entryStream.tellg() < totalSize) {
        uint32_t uncompressedSize;
        uint32_t compressedSize;

        entryStream.read(reinterpret_cast<char*>(&uncompressedSize), sizeof(uint32_t));
        entryStream.read(reinterpret_cast<char*>(&compressedSize), sizeof(uint32_t));

        vector<uint8_t> compressedBuffer(compressedSize);
        vector<uint8_t> decompressedBuffer(uncompressedSize);

        entryStream.read(reinterpret_cast<char*>(compressedBuffer.data()), compressedSize);
        uint32_t totalDecompressedSize = xpress9Wrapper.Decompress(compressedBuffer.data(), compressedSize, decompressedBuffer.data(), decompressedBuffer.size());

        if (totalDecompressedSize != uncompressedSize) {
            return "Decompressed size does not match expected size.";
        }

        allDecompressedData.insert(allDecompressedData.end(), decompressedBuffer.begin(), decompressedBuffer.end());
    }
    UnaryExecutor::Execute<string_t, string_t>(
        name_vector, result, args.size(),
        [&](string_t name) {
            std::string pbix_file = name.GetString();
            std::vector<uint8_t> pbix_buffer = AbfParser::read_file(pbix_file);
            std::vector<uint8_t> decompressed_buffer = Xpress9Wrapper::decompress(pbix_buffer);
            AbfParser::process_data(decompressed_buffer);
            return StringVector::AddString(result, "Pbix "+name.GetString()+" 🐥");;
        });
}


static void LoadInternal(DatabaseInstance &instance) {
    // Register a scalar function
    auto pbix_scalar_function = ScalarFunction("pbix", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PbixScalarFun);
    ExtensionUtil::RegisterFunction(instance, pbix_scalar_function);
    auto pbix_decompress_function = ScalarFunction("pbix_scan", {LogicalType::VARCHAR}, LogicalType::VARCHAR, DecompressAndProcessPbix);
    ExtensionUtil::RegisterFunction(instance, pbix_decompress_function);
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

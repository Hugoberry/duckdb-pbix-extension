#define DUCKDB_EXTENSION_MAIN

#include "pbix_extension.hpp"

#include "duckdb.hpp"
#include "duckdb/function/table_function.hpp"
#include "duckdb/common/exception.hpp"

#include <fstream>
#include <vector>

#include "duckdb/common/typedefs.hpp"
#include "duckdb/function/replacement_scan.hpp"
#include "duckdb/function/table_function.hpp"
#include "duckdb/main/extension_util.hpp"
#include "duckdb/parser/expression/constant_expression.hpp"
#include "duckdb/parser/expression/function_expression.hpp"
#include "duckdb/parser/tableref/table_function_ref.hpp"

#include "Xpress9Wrapper.h"
#include "AbfParser.h"

#include <tinyxml2.h>
#include <sqlite3.h>

using namespace duckdb;
using namespace std;

namespace duckdb
{

    extern XPress9Wrapper xpress9Wrapper;
    extern AbfParser parser;
    class PBIXScannerFunctionData : public TableFunctionData {
    public:
        ifstream entryStream;
        XPress9Wrapper xpress9Wrapper;
        int dataOffset = 102;
        streamsize totalSize;
        bool initialized = false;
        bool finished = false;
        PBIXScannerFunctionData(const string &fileToExtract) : entryStream(fileToExtract, ios::binary) {
            if (!entryStream) {
                throw IOException("Failed to open file: " + fileToExtract);
            }
            entryStream.seekg(0, ios::end);
            totalSize = entryStream.tellg();
            entryStream.seekg(dataOffset, ios::beg);

            if (!xpress9Wrapper.Initialize()) {
                throw IOException("Failed to initialize Xpress9Wrapper.");
            }
        }

        ~PBIXScannerFunctionData() {
            if (entryStream.is_open()) {
                entryStream.close();
            }
            xpress9Wrapper.Terminate();
        }
    };

    static unique_ptr<FunctionData> PBIXScannerBind(ClientContext &context, TableFunctionBindInput &input, vector<LogicalType> &return_types, vector<string> &names)
    {
        if (input.inputs.size() != 1 || input.inputs[0].type().id() != LogicalTypeId::VARCHAR)
        {
            throw std::runtime_error("PBIXScanner function requires a single VARCHAR argument (file path).");
        }

        auto file_path = input.inputs[0].GetValue<string>();
        auto bind_data = make_uniq<PBIXScannerFunctionData>(file_path);

        // Define the output schema. Adapt this based on what AbfParser produces.
        return_types.push_back(LogicalType::INTEGER); // e.g., ID column
        names.push_back("ID");
        // Add more columns based on the output of AbfParser
        
        return move(bind_data);
    }

    static void PBIXScannerFunction(ClientContext &context, TableFunctionInput &input, DataChunk &output) {
        auto &data = (PBIXScannerFunctionData &)*input.bind_data;

        if (data.finished || data.entryStream.tellg() >= data.totalSize) {
            data.finished = true;
            return; // No more data to process
        }

        // Process the file in chunks. Here we demonstrate processing a single chunk per call.
        // This loop should be adapted to your actual file structure and processing needs.
        uint32_t uncompressedSize, compressedSize;
        data.entryStream.read(reinterpret_cast<char*>(&uncompressedSize), sizeof(uint32_t));
        data.entryStream.read(reinterpret_cast<char*>(&compressedSize), sizeof(uint32_t));

        vector<uint8_t> compressedBuffer(compressedSize), decompressedBuffer(uncompressedSize);

        if (!data.entryStream.read(reinterpret_cast<char*>(compressedBuffer.data()), compressedSize)) {
            throw IOException("Failed to read compressed data.");
        }

        uint32_t totalDecompressedSize = data.xpress9Wrapper.Decompress(compressedBuffer.data(), compressedSize, decompressedBuffer.data(), decompressedBuffer.size());

        if (totalDecompressedSize != uncompressedSize) {
            throw IOException("Decompressed size does not match expected size.");
        }

        // At this point, `decompressedBuffer` contains the decompressed chunk of data.
        // You would then process this data to extract the rows and columns for DuckDB.

        // Example of processing decompressed data (this needs to be replaced with your actual data processing logic).
        // For demonstration, we pretend each decompressed chunk yields a single row with two columns.
        // AbfParser parser;
        // auto parsedData = parser.process_data(decompressedBuffer); // Assume this returns structured data we can use.

        // Assuming parsedData can be directly translated into rows for DuckDB:
        // for (auto &row : parsedData) {
        //     // Example of appending a row to the output DataChunk
        //     // Adapt the indices and types according to your actual data structure
        //     output.SetValue(0, output.size(), Value::INTEGER(row.id)); // Assuming a column "ID"
        //     output.SetValue(1, output.size(), Value(row.value)); // Assuming a column "Value"
        // }
        
        //rerttun the decompressed data
        output.SetValue(0, output.size(), Value::BLOB(decompressedBuffer.data(), decompressedBuffer.size()));

        // Mark the processing as finished if we've reached the end of the file.
        if (data.entryStream.tellg() >= data.totalSize) {
            data.finished = true;
        }
    }

    static void LoadInternal(DatabaseInstance &instance)
    {
        // Register a scalar function
        // auto pbix_scalar_function = ScalarFunction("pbix", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PbixScalarFun);
        // ExtensionUtil::RegisterFunction(instance, pbix_scalar_function);

        // auto pbix_scanner_scalar_function = ScalarFunction("pbix_scanner", {LogicalType::VARCHAR}, LogicalType::VARCHAR, PbixScannerScalarFun);
        // ExtensionUtil::RegisterFunction(instance, pbix_scanner_scalar_function);

        // create pbix scanner function
        auto pbix_scanner = TableFunction("pbix_scanner", {LogicalType::VARCHAR}, PBIXScannerFunction, PBIXScannerBind);
        ExtensionUtil::RegisterFunction(instance, pbix_scanner);
    }

    void PbixExtension::Load(DuckDB &db)
    {
        auto &db_instance = *db.instance;
        auto &fs = db.GetFileSystem();
        LoadInternal(db_instance);
    }
    std::string PbixExtension::Name()
    {
        return "pbix";
    }

} // namespace duckdb

extern "C"
{

    DUCKDB_EXTENSION_API void pbix_init(duckdb::DatabaseInstance &db)
    {
        duckdb::DuckDB db_wrapper(db);
        db_wrapper.LoadExtension<duckdb::PbixExtension>();
    }

    DUCKDB_EXTENSION_API const char *pbix_version()
    {
        return duckdb::DuckDB::LibraryVersion();
    }
}

#ifndef DUCKDB_EXTENSION_MAIN
#error DUCKDB_EXTENSION_MAIN not defined
#endif
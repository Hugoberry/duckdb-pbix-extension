#include "vpax_builder.hpp"
#include "vpax_schema.hpp"
#include "sqlite_db.hpp"
#include "abf_parser.h"
#include "duckdb.hpp"

using namespace duckdb;

void VpaxFunction::Execute(DataChunk &args, ExpressionState &state, Vector &result) {
    auto &file_name_vector = args.data[0];
    auto file_name_data = FlatVector::GetData<string_t>(file_name_vector);
    
    for (idx_t i = 0; i < args.size(); i++) {
        std::string file_name = file_name_data[i].GetString();
        
        try {
            // Extract SQLite database from PBIX file
            SQLiteDB db;
            SQLiteOpenOptions options;
            options.access_mode = AccessMode::READ_ONLY;
            
            // Get trailing chunks from config (default 15)
            int trailing_chunks = 15;
            Value magic_number;
            if (state.GetContext().TryGetCurrentSetting("pbix_magic_number", magic_number)) {
                trailing_chunks = IntegerValue::Get(magic_number);
            }
            
            // Parse PBIX file
            auto dataModel = AbfParser::get_sqlite(state.GetContext(), file_name, trailing_chunks);
            db = SQLiteDB::OpenFromBuffer(options, dataModel.metadata_db);
            
            // Build VPAX structure
            VpaxBuilder builder(db, file_name);
            Value vpax_result = builder.BuildVpax();
            
            // Set result
            auto result_data = FlatVector::GetData<struct_entry_t>(result);
            result_data[i] = vpax_result.GetValue<struct_entry_t>();
            
        } catch (const std::exception &e) {
            // Return error structure
            Value error_result = VpaxValueFactory::CreateErrorValue(e.what(), file_name, "Execute");
            
            auto result_data = FlatVector::GetData<struct_entry_t>(result);
            result_data[i] = error_result.GetValue<struct_entry_t>();
        }
    }
}

std::unique_ptr<FunctionData> VpaxFunction::Bind(
    ClientContext &context, 
    ScalarFunction &bound_function,
    std::vector<std::unique_ptr<Expression>> &arguments) {
    
    // Set the return type to the complete VPAX schema
    bound_function.return_type = VpaxSchema::CreateVpaxType();
    
    return nullptr; // No special function data needed
}

void VpaxFunction::Register(ExtensionLoader &loader) {
    // Create and register the pbix2vpax scalar function
    ScalarFunction pbix2vpax(
        "pbix2vpax",                           // Function name
        {LogicalType::VARCHAR},                 // Input: filename
        LogicalType::STRUCT({}),               // Output: Will be set in bind function
        VpaxFunction::Execute,                 // Implementation
        VpaxFunction::Bind                     // Bind function
    );
    
    // Mark as having side effects (file I/O)
    // pbix2vpax.side_effects = FunctionSideEffects::HAS_SIDE_EFFECTS;
    
    // Register with DuckDB
    loader.RegisterFunction(pbix2vpax);
}
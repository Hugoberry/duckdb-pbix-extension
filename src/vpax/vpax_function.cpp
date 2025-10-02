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
            VpaxBuilder builder(db, file_name, dataModel.vertipaq_files);
            Value vpax_result = builder.BuildVpax();
            
            // Set result
            result.SetValue(i, vpax_result);
            
        } catch (const std::exception &e) {
           // Create empty VPAX structure on error (maintains type consistency)
           throw InvalidInputException("Failed to call pbix2vpax() for file '%s': %s", 
                                      file_name.c_str(), e.what()); 
        }
    }
}

unique_ptr<FunctionData> VpaxFunction::Bind(
    ClientContext &context, 
    ScalarFunction &bound_function,
    vector<unique_ptr<Expression>> &arguments) {
    
    // Set the return type to the complete VPAX schema
    bound_function.return_type = VpaxSchema::CreateVpaxType();
    
    return nullptr; // No special function data needed
}

void VpaxFunction::Register(ExtensionLoader &loader) {
    // Create and register the pbix2vpax scalar function
    ScalarFunction pbix2vpax(
        "pbix2vpax",                           // Function name
        {LogicalType::VARCHAR},                 // Input: filename
        VpaxSchema::CreateVpaxType(),               // Output: Will be set in bind function
        VpaxFunction::Execute                 // Implementation
    );
    
    // pbix2vpax.bind = VpaxFunction::Bind;
    // Mark as having side effects (file I/O)
    // pbix2vpax.side_effects = FunctionSideEffects::HAS_SIDE_EFFECTS;
    
    // Register with DuckDB
    loader.RegisterFunction(pbix2vpax);
}
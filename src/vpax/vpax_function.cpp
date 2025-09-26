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
            result.SetValue(i, vpax_result);
            
        } catch (const std::exception &e) {
           // Create empty VPAX structure on error (maintains type consistency)
            child_list_t<Value> empty_vpax_values;
            empty_vpax_values.push_back(make_pair("Tables", Value::LIST(VpaxSchema::CreateTableType(), std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("Columns", Value::LIST(VpaxSchema::CreateColumnType(), std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("Measures", Value::LIST(VpaxSchema::CreateMeasureType(), std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("ColumnsSegments", Value::LIST(VpaxSchema::CreateColumnSegmentType(), std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("ColumnsHierarchies", Value::LIST(VpaxSchema::CreateColumnHierarchyType(), std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("UserHierarchies", Value::LIST(VpaxSchema::CreateUserHierarchyType(), std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("Relationships", Value::LIST(VpaxSchema::CreateRelationshipType(), std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("TablePermissions", Value::LIST(LogicalType::VARCHAR, std::vector<Value>())));
            empty_vpax_values.push_back(make_pair("CalculationItems", Value::LIST(LogicalType::VARCHAR, std::vector<Value>())));
            
            Value empty_vpax = Value::STRUCT(empty_vpax_values);
            result.SetValue(i, empty_vpax);
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
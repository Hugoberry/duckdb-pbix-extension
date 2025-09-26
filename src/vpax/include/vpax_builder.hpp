#pragma once

#include "duckdb.hpp"
#include "sqlite_db.hpp"
#include "vpax_types.hpp"
#include <vector>

namespace duckdb {

// Forward declarations
class SQLiteDB;

// Main VPAX Builder class
class VpaxBuilder {
private:
    SQLiteDB &db_;
    std::string file_name_;
    
public:
    VpaxBuilder(SQLiteDB &db, const std::string &file_name);
    ~VpaxBuilder() = default;
    
    // Main build function
    Value BuildVpax();
    
    // Section builders
    std::vector<Value> BuildTables();
    std::vector<Value> BuildColumns();
    std::vector<Value> BuildMeasures();
    std::vector<Value> BuildRelationships();
    std::vector<Value> BuildColumnSegments();
    std::vector<Value> BuildColumnHierarchies();
    std::vector<Value> BuildUserHierarchies();
    std::vector<Value> BuildTablePermissions();  // Empty for now
    std::vector<Value> BuildCalculationItems();  // Empty for now
    
private:
    // Helper functions
    int64_t CalculateTableSize(const std::string &table_name);
    int64_t CalculateColumnSize(const std::string &table_name, const std::string &column_name);
    int64_t CalculateRelationshipSize(const std::string &from_table, const std::string &to_table);
    double CalculateSelectivity(const std::string &table_name, const std::string &column_name);
    
    // Error handling
    Value CreateErrorResult(const std::exception &e);
};

// VPAX Function Registration
class VpaxFunction {
public:
    // Main scalar function implementation
    static void Execute(DataChunk &args, ExpressionState &state, Vector &result);
    
    // Function binding
    static unique_ptr<FunctionData> Bind(
        ClientContext &context, 
        ScalarFunction &bound_function,
        vector<unique_ptr<Expression>> &arguments
    );
    
    // Register with DuckDB
    static void Register(ExtensionLoader &loader);
};

} // namespace duckdb
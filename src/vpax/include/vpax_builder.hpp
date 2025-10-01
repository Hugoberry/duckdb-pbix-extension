#pragma once

#include "duckdb.hpp"
#include "sqlite_db.hpp"
#include "vpax_types.hpp"
#include "vertipaq_meta.hpp"
#include <unordered_map>
#include <vector>

namespace duckdb {

// Forward declarations
class SQLiteDB;

// Main VPAX Builder class
class VpaxBuilder {
private:
    SQLiteDB &db_;
    std::string file_name_;
    std::unordered_map<std::string, int64_t> file_size_map_;
    
public:
    VpaxBuilder(SQLiteDB &db, const std::string &file_name,
               const std::vector<VertipaqFile> &vertipaq_files);
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
    int64_t GetFileSizeByName(const std::string &filename);
    int64_t CalculateTableColumnsSize(int table_id);
    int64_t CalculateTableHierarchiesSize(int table_id);
    int64_t CalculateColumnHierarchySize(int table_id, int column_id);
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
#pragma once

#include "duckdb.hpp"
#include "sqlite_db.hpp"
#include "vpax_types.hpp"
#include "vertipaq_meta.hpp"
#include <unordered_map>
#include <vector>

namespace duckdb {

class SQLiteDB;

// Structure to hold pre-fetched data
struct PreFetchedData {
    // Table files: table_id -> list of filenames
    std::unordered_map<int, std::vector<std::string>> table_files;
    
    // Dictionary sizes: column_id -> dictionary size
    std::unordered_map<int, int64_t> dictionary_sizes;
    
    // Column files: column_id -> filename
    std::unordered_map<int, std::string> column_files;
    
    // Table hierarchies files: table_id -> list of filenames
    std::unordered_map<int, std::vector<std::string>> table_hierarchy_files;
    
    // Column hierarchy files: column_id -> list of filenames
    std::unordered_map<int, std::vector<std::string>> column_hierarchy_files;
    
    // User hierarchy files: hierarchy_id -> list of filenames
    std::unordered_map<int, std::vector<std::string>> user_hierarchy_files;
    
    // Relationship counts: from_table_name -> count
    std::unordered_map<std::string, int> relationship_counts;
};

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
    
    // Pre-fetch all data
    void PreFetchData(PreFetchedData &data);
    
    // Section builders (updated to use pre-fetched data)
    std::vector<Value> BuildTables(const PreFetchedData &prefetched);
    std::vector<Value> BuildColumns(const PreFetchedData &prefetched);
    std::vector<Value> BuildMeasures();
    std::vector<Value> BuildRelationships();
    std::vector<Value> BuildColumnSegments();
    std::vector<Value> BuildColumnHierarchies();
    std::vector<Value> BuildUserHierarchies(const PreFetchedData &prefetched);
    std::vector<Value> BuildPartitions();
    std::vector<Value> BuildTablePermissions();
    std::vector<Value> BuildCalculationItems();
    
private:
    // Helper functions (updated to use pre-fetched data)
    int64_t GetFileSizeByName(const std::string &filename);
    int64_t CalculateTableColumnsSize(int table_id, const PreFetchedData &prefetched);
    int64_t CalculateTableHierarchiesSize(int table_id, const PreFetchedData &prefetched);
    int64_t CalculateColumnHierarchySize(int column_id, const PreFetchedData &prefetched);
    int64_t CalculateUserHierarchySize(int hierarchy_id, const PreFetchedData &prefetched);
    int64_t CalculateRelationshipSize(const std::string &from_table, const std::string &to_table);
    double CalculateSelectivity(const std::string &table_name, const std::string &column_name);
    
    // Error handling
    Value CreateErrorResult(const std::exception &e);
};

// VPAX Function Registration
class VpaxFunction {
public:
    static void Execute(DataChunk &args, ExpressionState &state, Vector &result);
    static unique_ptr<FunctionData> Bind(
        ClientContext &context, 
        ScalarFunction &bound_function,
        vector<unique_ptr<Expression>> &arguments
    );
    static void Register(ExtensionLoader &loader);
};

} // namespace duckdb
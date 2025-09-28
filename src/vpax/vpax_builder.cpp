#include "vpax_builder.hpp"
#include "vpax_schema.hpp"
#include "vpax_types.hpp"
#include "sqlite_db.hpp"
#include "sqlite_stmt.hpp"
#include "abf_parser.h"

using namespace duckdb;

VpaxBuilder::VpaxBuilder(SQLiteDB &db, const std::string &file_name) 
    : db_(db), file_name_(file_name) {
}

Value VpaxBuilder::BuildVpax() {
    try {
        // Build all sections
        auto tables = BuildTables();
        auto columns = BuildColumns();
        // auto measures = BuildMeasures();
        // auto relationships = BuildRelationships();
        // auto segments = BuildColumnSegments();
        // auto column_hierarchies = BuildColumnHierarchies();
        // auto user_hierarchies = BuildUserHierarchies();
        // auto table_permissions = BuildTablePermissions();
        // auto calc_items = BuildCalculationItems();
        
        // Create the main VPAX structure
        child_list_t<Value> vpax_values;
        vpax_values.push_back(make_pair("Tables", Value::LIST(VpaxSchema::CreateTableType(), vector<Value>(tables.begin(), tables.end()))));
        vpax_values.push_back(make_pair("Columns", Value::LIST(VpaxSchema::CreateColumnType(), vector<Value>(columns.begin(), columns.end()))));
        // vpax_values.push_back(make_pair("Measures", Value::LIST(VpaxSchema::CreateMeasureType(), vector<Value>(measures.begin(), measures.end()))));
        // vpax_values.push_back(make_pair("ColumnsSegments", Value::LIST(VpaxSchema::CreateColumnSegmentType(), vector<Value>(segments.begin(), segments.end()))));
        // vpax_values.push_back(make_pair("ColumnsHierarchies", Value::LIST(VpaxSchema::CreateColumnHierarchyType(), vector<Value>(column_hierarchies.begin(), column_hierarchies.end()))));
        // vpax_values.push_back(make_pair("UserHierarchies", Value::LIST(VpaxSchema::CreateUserHierarchyType(), vector<Value>(user_hierarchies.begin(), user_hierarchies.end()))));
        // vpax_values.push_back(make_pair("Relationships", Value::LIST(VpaxSchema::CreateRelationshipType(), vector<Value>(relationships.begin(), relationships.end()))));
        // vpax_values.push_back(make_pair("TablePermissions", Value::LIST(LogicalType::VARCHAR, vector<Value>(table_permissions.begin(), table_permissions.end()))));
        // vpax_values.push_back(make_pair("CalculationItems", Value::LIST(LogicalType::VARCHAR, vector<Value>(calc_items.begin(), calc_items.end()))));
        // vpax_values.push_back(make_pair("Columns", Value::LIST(VpaxSchema::CreateColumnType(), std::vector<Value>())));
        vpax_values.push_back(make_pair("Measures", Value::LIST(VpaxSchema::CreateMeasureType(), std::vector<Value>())));
        vpax_values.push_back(make_pair("ColumnsSegments", Value::LIST(VpaxSchema::CreateColumnSegmentType(), std::vector<Value>())));
        vpax_values.push_back(make_pair("ColumnsHierarchies", Value::LIST(VpaxSchema::CreateColumnHierarchyType(), std::vector<Value>())));
        vpax_values.push_back(make_pair("UserHierarchies", Value::LIST(VpaxSchema::CreateUserHierarchyType(), std::vector<Value>())));
        vpax_values.push_back(make_pair("Relationships", Value::LIST(VpaxSchema::CreateRelationshipType(), std::vector<Value>())));
        vpax_values.push_back(make_pair("TablePermissions", Value::LIST(LogicalType::VARCHAR, std::vector<Value>())));
        vpax_values.push_back(make_pair("CalculationItems", Value::LIST(LogicalType::VARCHAR, std::vector<Value>())));
        
        return Value::STRUCT(vpax_values);
        
   } catch (const std::exception &e) {
        // This ensures we always return the expected VPAX type
        
        std::cout << "Error in BuildVpax: " << e.what() << std::endl;
        
        // Create empty VPAX structure with correct type
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
        
        return Value::STRUCT(empty_vpax_values);
    }
}

std::vector<Value> VpaxBuilder::BuildTables() {
    std::vector<Value> tables;
    
    std::string sql = R"(
        SELECT 
            t.Name as TableName,
            t.isHidden,
            666 as RowsCount,
            '' as Description
        FROM [Table] t where ishidden = 0;
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string table_name = stmt.GetValue<std::string>(0);
        bool is_hidden = stmt.GetValue<int>(1) != 0;
        int64_t row_count = stmt.GetValue<int64_t>(2);
        std::string description = stmt.GetValue<std::string>(3);
        
        // Calculate sizes
        int64_t columns_size = CalculateTableSize(table_name);
        int64_t table_size = columns_size; // For now, same as columns size
        int64_t rel_size = CalculateRelationshipSize(table_name, "");
        bool is_referenced = VpaxUtils::CheckIfTableIsReferenced(db_, table_name);
        
        tables.push_back(VpaxValueFactory::CreateTableValue(
            table_name, row_count, is_hidden, columns_size, 
            table_size, rel_size, 0, is_referenced, description
        ));
    }
    
    return tables;
}

std::vector<Value> VpaxBuilder::BuildColumns() {
    std::vector<Value> columns;
    
    std::string sql = R"(
        SELECT 
            c.ExplicitName as ColumnName,
            t.Name as TableName,
            c.ExplicitDataType,
            c.isHidden,
            c.isKey,
            c.isNullable,
            c.isUnique,
            COALESCE(c.Description, '') as Description,
            COALESCE(c.FormatString, '') as FormatString,
            0 as Cardinality,
            666 as TotalSize,
            666 as DictionarySize
        FROM COLUMN c
        JOIN [Table] t ON c.TableId = t.ID
        LEFT JOIN ColumnStorage cs ON c.ColumnStorageID = cs.ID
        LEFT JOIN DictionaryStorage ds ON cs.DictionaryStorageID = ds.ID
        WHERE c.Type = 1  -- Data columns only
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string column_name = stmt.GetValue<std::string>(0);
        std::string table_name = stmt.GetValue<std::string>(1);
        int data_type_id = stmt.GetValue<int>(2);
        bool is_hidden = stmt.GetValue<int>(3) != 0;
        bool is_key = stmt.GetValue<int>(4) != 0;
        bool is_nullable = stmt.GetValue<int>(5) != 0;
        bool is_unique = stmt.GetValue<int>(6) != 0;
        std::string description = stmt.GetValue<std::string>(7);
        std::string format_string = stmt.GetValue<std::string>(8);
        int64_t cardinality = stmt.GetValue<int64_t>(9);
        int64_t total_size = stmt.GetValue<int64_t>(10);
        int64_t dictionary_size = stmt.GetValue<int64_t>(11);
        
        std::string data_type = VpaxUtils::DataTypeIdToString(data_type_id);
        int64_t data_size = total_size - dictionary_size;
        double selectivity = CalculateSelectivity(table_name, column_name);
        
        columns.push_back(VpaxValueFactory::CreateColumnValue(
            column_name, table_name, data_type, is_hidden, cardinality,
            total_size, dictionary_size, data_size, is_key, is_nullable,
            is_unique, "Hash", description, format_string, selectivity
        ));
    }
    
    return columns;
}

std::vector<Value> VpaxBuilder::BuildMeasures() {
    std::vector<Value> measures;
    
    std::string sql = R"(
        SELECT 
            m.Name as MeasureName,
            t.Name as TableName,
            COALESCE(m.Expression, '') as Expression,
            m.isHidden,
            m.DataType,
            COALESCE(m.Description, '') as Description,
            COALESCE(m.FormatString, '') as FormatString
        FROM Measure m
        JOIN [Table] t ON m.TableID = t.ID
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string measure_name = stmt.GetValue<std::string>(0);
        std::string table_name = stmt.GetValue<std::string>(1);
        std::string expression = stmt.GetValue<std::string>(2);
        bool is_hidden = stmt.GetValue<int>(3) != 0;
        int data_type_id = stmt.GetValue<int>(4);
        std::string description = stmt.GetValue<std::string>(5);
        std::string format_string = stmt.GetValue<std::string>(6);
        
        std::string data_type = VpaxUtils::DataTypeIdToString(data_type_id);
        
        measures.push_back(VpaxValueFactory::CreateMeasureValue(
            measure_name, table_name, expression, is_hidden,
            data_type, description, format_string, false
        ));
    }
    
    return measures;
}

std::vector<Value> VpaxBuilder::BuildRelationships() {
    std::vector<Value> relationships;
    
    std::string sql = R"(
        SELECT 
            tf.Name as FromTableName,
            cf.ExplicitName as FromColumnName,
            tt.Name as ToTableName,
            ct.ExplicitName as ToColumnName,
            r.IsActive,
            r.CrossFilteringBehavior,
            r.RelyOnReferentialIntegrity,
            COALESCE(r.JoinOnDateBehavior, 0) as JoinOnDateBehavior
        FROM Relationship r
        JOIN COLUMN cf ON r.FromColumnID = cf.ID
        JOIN [Table] tf ON cf.TableId = tf.ID
        JOIN COLUMN ct ON r.ToColumnID = ct.ID
        JOIN [Table] tt ON ct.TableId = tt.ID
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string from_table = stmt.GetValue<std::string>(0);
        std::string from_column = stmt.GetValue<std::string>(1);
        std::string to_table = stmt.GetValue<std::string>(2);
        std::string to_column = stmt.GetValue<std::string>(3);
        bool is_active = stmt.GetValue<int>(4) != 0;
        int cross_filter = stmt.GetValue<int>(5);
        bool rely_on_ri = stmt.GetValue<int>(6) != 0;
        
        std::string cross_filtering = "OneDirection";
        if (cross_filter == 2) cross_filtering = "BothDirections";
        
        relationships.push_back(VpaxValueFactory::CreateRelationshipValue(
            from_table, from_column, to_table, to_column, is_active,
            0, 0, cross_filtering, "Regular", rely_on_ri
        ));
    }
    
    return relationships;
}

std::vector<Value> VpaxBuilder::BuildColumnSegments() {
    std::vector<Value> segments;
    
    // This is a simplified implementation
    // In a real scenario, you'd query partition and segment metadata
    std::string sql = R"(
        SELECT 
            c.ExplicitName as ColumnName,
            t.Name as TableName,
            COALESCE(p.Name, 'Partition') as PartitionName,
            COALESCE(cs.SegmentNumber, 0) as SegmentNumber,
            666 as UsedSize
        FROM COLUMN c
        JOIN [Table] t ON c.TableId = t.ID
        LEFT JOIN ColumnStorage cs ON c.ColumnStorageID = cs.ID
        LEFT JOIN Partition p ON p.TableID = t.ID
        WHERE c.Type = 1
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string column_name = stmt.GetValue<std::string>(0);
        std::string table_name = stmt.GetValue<std::string>(1);
        std::string partition_name = stmt.GetValue<std::string>(2);
        int segment_number = stmt.GetValue<int>(3);
        int64_t used_size = stmt.GetValue<int64_t>(4);
        
        segments.push_back(VpaxValueFactory::CreateColumnSegmentValue(
            column_name, table_name, partition_name, segment_number,
            0, 0, used_size, "Hash", 0, 0, "Ready"
        ));
    }
    
    return segments;
}

std::vector<Value> VpaxBuilder::BuildColumnHierarchies() {
    // TODO: Implement when hierarchy metadata is available
    return std::vector<Value>();
}

std::vector<Value> VpaxBuilder::BuildUserHierarchies() {
    std::vector<Value> hierarchies;
    
    std::string sql = R"(
        SELECT 
            t.Name as TableName,
            h.Name as HierarchyName,
            h.isHidden,
            COALESCE(h.UsedSize, 0) as UsedSize
        FROM Hierarchy h
        JOIN [Table] t ON h.TableID = t.ID
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string table_name = stmt.GetValue<std::string>(0);
        std::string hierarchy_name = stmt.GetValue<std::string>(1);
        bool is_hidden = stmt.GetValue<int>(2) != 0;
        int64_t used_size = stmt.GetValue<int64_t>(3);
        
        // Get hierarchy levels
        std::string levels_sql = R"(
            SELECT c.ExplicitName
            FROM Level l
            JOIN COLUMN c ON l.ColumnID = c.ID
            JOIN Hierarchy h ON l.HierarchyID = h.ID
            WHERE h.Name = ? AND h.TableID = (SELECT ID FROM [Table] WHERE Name = ?)
            ORDER BY l.Ordinal
        )";
        
        SQLiteStatement levels_stmt = db_.Prepare(levels_sql);
        levels_stmt.BindText(0, string_t(hierarchy_name));
        levels_stmt.BindText(1, string_t(table_name));
        
        std::vector<std::string> level_names;
        while (levels_stmt.Step()) {
            level_names.push_back(levels_stmt.GetValue<std::string>(0));
        }
        
        // Build levels string manually
        std::string levels_string = "";
        for (size_t i = 0; i < level_names.size(); i++) {
            if (i > 0) levels_string += ", ";
            levels_string += level_names[i];
        }
        
        hierarchies.push_back(VpaxValueFactory::CreateUserHierarchyValue(
            table_name, hierarchy_name, is_hidden, used_size, levels_string
        ));
    }
    
    return hierarchies;
}

std::vector<Value> VpaxBuilder::BuildTablePermissions() {
    // Empty for now - would require security metadata
    return std::vector<Value>();
}

std::vector<Value> VpaxBuilder::BuildCalculationItems() {
    // Empty for now - would require calculation group metadata  
    return std::vector<Value>();
}

// Helper implementations
int64_t VpaxBuilder::CalculateTableSize(const std::string &table_name) {
    std::string sql = R"(
        SELECT 666
        FROM COLUMN c
        JOIN ColumnStorage cs ON c.ColumnStorageID = cs.ID
        JOIN [Table] t ON c.TableId = t.ID
        WHERE t.Name = ? AND c.Type = 1
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    stmt.BindText(0, string_t(table_name));
    
    if (stmt.Step()) {
        return stmt.GetValue<int64_t>(0);
    }
    return 0;
}

int64_t VpaxBuilder::CalculateColumnSize(const std::string &table_name, const std::string &column_name) {
    // Implementation would depend on storage metadata availability
    return 0;
}

int64_t VpaxBuilder::CalculateRelationshipSize(const std::string &from_table, const std::string &to_table) {
    // Implementation would depend on relationship storage metadata
    return 0;
}

double VpaxBuilder::CalculateSelectivity(const std::string &table_name, const std::string &column_name) {
    // Implementation would calculate cardinality / row_count
    return 0.0;
}

Value VpaxBuilder::CreateErrorResult(const std::exception &e) {
    return VpaxValueFactory::CreateErrorValue(e.what(), file_name_, "BuildVpax");
}
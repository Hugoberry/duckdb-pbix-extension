#include "vpax_builder.hpp"
#include "vpax_schema.hpp"
#include "vpax_types.hpp"
#include "sqlite_db.hpp"
#include "sqlite_stmt.hpp"
#include "abf_parser.h"

using namespace duckdb;

VpaxBuilder::VpaxBuilder(SQLiteDB &db, const std::string &file_name,
                         const std::vector<VertipaqFile> &vertipaq_files) 
    : db_(db), file_name_(file_name) {
    // Build file size map for quick lookup
    for (const auto &vfile : vertipaq_files) {
        file_size_map_[vfile.FileName] = vfile.Size;
    }        
}

int64_t VpaxBuilder::GetFileSizeByName(const std::string &filename) {
    auto it = file_size_map_.find(filename);
    return (it != file_size_map_.end()) ? it->second : 0;
}

Value VpaxBuilder::BuildVpax() {
    try {
        // Build all sections
        auto tables = BuildTables();
        auto columns = BuildColumns();
        auto measures = BuildMeasures();
        auto relationships = BuildRelationships();
        // auto segments = BuildColumnSegments();
        auto column_hierarchies = BuildColumnHierarchies();
        auto user_hierarchies = BuildUserHierarchies();
        auto partitions = BuildPartitions();
        // auto table_permissions = BuildTablePermissions();
        // auto calc_items = BuildCalculationItems();
        
        // Create the main VPAX structure
        child_list_t<Value> vpax_values;
        vpax_values.push_back(make_pair("Tables", Value::LIST(VpaxSchema::CreateTableType(), vector<Value>(tables.begin(), tables.end()))));
        vpax_values.push_back(make_pair("Columns", Value::LIST(VpaxSchema::CreateColumnType(), vector<Value>(columns.begin(), columns.end()))));
        vpax_values.push_back(make_pair("Measures", Value::LIST(VpaxSchema::CreateMeasureType(), vector<Value>(measures.begin(), measures.end()))));
        // vpax_values.push_back(make_pair("ColumnsSegments", Value::LIST(VpaxSchema::CreateColumnSegmentType(), vector<Value>(segments.begin(), segments.end()))));
        vpax_values.push_back(make_pair("ColumnsHierarchies", Value::LIST(VpaxSchema::CreateColumnHierarchyType(), vector<Value>(column_hierarchies.begin(), column_hierarchies.end()))));
        vpax_values.push_back(make_pair("UserHierarchies", Value::LIST(VpaxSchema::CreateUserHierarchyType(), vector<Value>(user_hierarchies.begin(), user_hierarchies.end()))));
        vpax_values.push_back(make_pair("Relationships", Value::LIST(VpaxSchema::CreateRelationshipType(), vector<Value>(relationships.begin(), relationships.end()))));
        vpax_values.push_back(make_pair("Partitions", Value::LIST(VpaxSchema::CreatePartitionType(), vector<Value>(partitions.begin(), partitions.end()))));
        // vpax_values.push_back(make_pair("TablePermissions", Value::LIST(LogicalType::VARCHAR, vector<Value>(table_permissions.begin(), table_permissions.end()))));
        // vpax_values.push_back(make_pair("CalculationItems", Value::LIST(LogicalType::VARCHAR, vector<Value>(calc_items.begin(), calc_items.end()))));
        vpax_values.push_back(make_pair("ColumnsSegments", Value::LIST(VpaxSchema::CreateColumnSegmentType(), std::vector<Value>())));
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
        empty_vpax_values.push_back(make_pair("Partitions", Value::LIST(VpaxSchema::CreatePartitionType(), std::vector<Value>())));
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
            t.IsHidden,
            cs.Statistics_DistinctStates as RowsCount,
            t.IsPrivate,
            t.DataCategory,
            t.Description,
            ts.RIViolationCount,
            t.ID
        FROM [table] t
        LEFT JOIN [Column] c on t.ID = C.TableID
        LEFT JOIN [ColumnStorage] cs on c.ID = cs.ColumnID
        LEFT JOIN [TableStorage] ts on c.TableID = ts.TableID
        WHERE t.systemflags = 0 and c.isKey = 1
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string table_name = stmt.GetValue<std::string>(0);
        bool is_hidden = stmt.GetValue<int>(1) != 0;
        int64_t row_count = stmt.GetValue<int64_t>(2);
        bool is_private = stmt.GetValue<int>(3) != 0;
        std::string data_category = stmt.GetValue<std::string>(4);
        std::string description = stmt.GetValue<std::string>(5);
        int64_t ri_violation_count = stmt.GetValue<int64_t>(6);
        int table_id = stmt.GetValue<int>(7);
        
        // Calculate actual sizes
        int64_t columns_size = CalculateTableColumnsSize(table_id);
        int64_t hierarchies_size = CalculateTableHierarchiesSize(table_id);
        int64_t table_size = columns_size + hierarchies_size;
        int64_t rel_size = CalculateRelationshipSize(table_name, "");
        bool is_referenced = VpaxUtils::CheckIfTableIsReferenced(db_, table_name);
        
        tables.push_back(VpaxValueFactory::CreateTableValue(
            table_name, row_count, is_hidden, is_private, columns_size,
            table_size, rel_size, hierarchies_size, is_referenced, ri_violation_count, description
        ));
    }
    
    return tables;
}

int64_t VpaxBuilder::CalculateTableColumnsSize(int table_id) {
    std::string sql = R"(
        WITH TableFiles AS (
        /*    SELECT sf.id AS FolderID
            FROM [Table] t
            INNER JOIN TableStorage ts ON t.ID = ts.TableID
            INNER JOIN StorageFolder sf ON sf.id = ts.StorageFolderID
            WHERE t.id = ?
            
            UNION
            
        */  SELECT ps.StorageFolderID AS FolderID
            FROM [Table] t
            INNER JOIN [Partition] p ON p.tableid = t.id
            INNER JOIN PartitionStorage ps ON ps.partitionid = p.ID
            WHERE t.id = ?
            
            UNION
            
            SELECT sf.ID
            FROM COLUMN c
            INNER JOIN [Table] t ON c.TableId = t.ID
            INNER JOIN AttributeHierarchy ah ON ah.ColumnID = c.ID
            INNER JOIN AttributeHierarchyStorage ahs ON ah.AttributeHierarchyStorageID = ahs.ID
            INNER JOIN [Table] st ON st.id = ahs.SystemTableID
            INNER JOIN [Partition] p ON p.tableid = st.id
            INNER JOIN PartitionStorage ps ON ps.partitionid = p.id
            INNER JOIN StorageFolder sf ON sf.ID = ps.StorageFolderID
            WHERE t.ID = ?
        )
        SELECT DISTINCT
            sff.FileName
        FROM TableFiles tf
        INNER JOIN StorageFile sff ON sff.StorageFolderID = tf.FolderID
        CROSS JOIN [Table] t
        WHERE t.id = ?;
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    stmt.Bind(0, table_id);
    stmt.Bind(1, table_id);
    stmt.Bind(2, table_id);
    // stmt.Bind(3, table_id);
    
    int64_t total = 0;
    while (stmt.Step()) {
        std::string data_filename = stmt.GetValue<std::string>(0);
        total += GetFileSizeByName(data_filename);
    }

    SQLiteStatement stmt2 = db_.Prepare(R"(
        select sum(ds.size) 
        from column c 
        join ColumnStorage cs on c.id = cs.ColumnID
        JOIN DictionaryStorage ds on ds.ColumnStorageID = cs.ID
        where c.TableID = ?;
    )");

    stmt2.Bind(0, table_id);
    while (stmt2.Step()) {
        int64_t dict_size = stmt2.GetValue<int64_t>(0);
        total += dict_size;
    }

    return total;
}

int64_t VpaxBuilder::CalculateTableHierarchiesSize(int table_id) {
    std::string sql = R"(
        SELECT DISTINCT sff.FileName
        FROM Hierarchy h
        INNER JOIN [Table] t 
            ON h.TableID = t.ID
        INNER JOIN HierarchyStorage hs 
            ON hs.HierarchyID = h.id
        INNER JOIN [Table] st 
            ON st.id = hs.SystemTableID
        INNER JOIN [Partition] p 
            ON p.tableid = st.id
        INNER JOIN PartitionStorage ps 
            ON ps.partitionid = p.id
        INNER JOIN StorageFolder sf 
            ON sf.ID = ps.StorageFolderID
        INNER JOIN StorageFile sff 
            ON sf.id = sff.StorageFolderID
        WHERE t.ID = ?;
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    stmt.Bind(0, table_id);
    
    int64_t total = 0;
    while (stmt.Step()) {
        std::string hidx_filename = stmt.GetValue<std::string>(0);
        total += GetFileSizeByName(hidx_filename);
    }
    return total;
}

int64_t VpaxBuilder::CalculateColumnHierarchySize(int table_id, int column_id) {
    std::string sql = R"(
        SELECT DISTINCT sff.FileName
        FROM COLUMN c
        INNER JOIN [Table] t 
            ON c.TableId = t.ID
        INNER JOIN AttributeHierarchy ah 
            ON ah.ColumnID = c.ID
        INNER JOIN AttributeHierarchyStorage ahs 
            ON ah.AttributeHierarchyStorageID = ahs.ID
        INNER JOIN [Table] st 
            ON st.id = ahs.SystemTableID
        INNER JOIN [Partition] p 
            ON p.tableid = st.id
        INNER JOIN PartitionStorage ps 
            ON ps.partitionid = p.id
        INNER JOIN StorageFolder sf 
            ON sf.ID = ps.StorageFolderID
        INNER JOIN StorageFile sff 
            ON sf.id = sff.StorageFolderID
        WHERE t.ID = ? and c.ID = ?;
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    stmt.Bind(0, table_id);
    stmt.Bind(1, column_id);
    
    int64_t total = 0;
    while (stmt.Step()) {
        std::string hidx_filename = stmt.GetValue<std::string>(0);
        total += GetFileSizeByName(hidx_filename);
    }
    return total;
}


int64_t VpaxBuilder::CalculateUserHierarchySize(int hierarchy_id) {
    std::string sql = R"(
        SELECT DISTINCT sff.FileName
        FROM Hierarchy h
        INNER JOIN HierarchyStorage hs 
            ON hs.HierarchyID = h.id
        INNER JOIN [Table] st 
            ON st.id = hs.SystemTableID
        INNER JOIN [Partition] p 
            ON p.tableid = st.id
        INNER JOIN PartitionStorage ps 
            ON ps.partitionid = p.id
        INNER JOIN StorageFolder sf 
            ON sf.ID = ps.StorageFolderID
        INNER JOIN StorageFile sff 
            ON sf.id = sff.StorageFolderID
        WHERE h.ID=?;
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    stmt.Bind(0, hierarchy_id);
    
    int64_t total = 0;
    while (stmt.Step()) {
        std::string hierarchy_filename = stmt.GetValue<std::string>(0);
        total += GetFileSizeByName(hierarchy_filename);
    }
    return total;
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
            c.Description,
            c.FormatString,
            COALESCE(cs.Statistics_DistinctStates,0) as Cardinality,
            c.DisplayFolder,
            c.Expression,
            CAST(c.EncodingHint as VARCHAR) as EncodingHint,
            c.KeepUniqueRows,
            CAST(c.State as VARCHAR) as State,
            c.IsAvailableInMDX,
            ds.Size as DictionarySize,
            sfi.FileName AS IDFFileName,
            case WHEN ds.StorageFileID=0 THEN 'VALUE' ELSE 'HASH' END AS Encoding,
            c.ID,
            t.ID as TableID
        FROM COLUMN c
        JOIN [Table] t ON c.TableId = t.ID
        JOIN ColumnStorage cs ON c.ColumnStorageID = cs.ID
        -- Dictionary
        LEFT JOIN DictionaryStorage ds ON cs.DictionaryStorageID = ds.ID
        -- IDF (Index/Data File)
        LEFT JOIN ColumnPartitionStorage cps ON cps.ColumnStorageID = cs.ID
        LEFT JOIN StorageFile sfi ON sfi.ID = cps.StorageFileID
        WHERE t.systemflags = 0
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
        std::string display_folder = stmt.GetValue<std::string>(10);
        std::string expression = stmt.GetValue<std::string>(11);
        std::string encoding_hint = stmt.GetValue<std::string>(12);
        bool keep_unique_rows = stmt.GetValue<int>(13) != 0;
        std::string state = stmt.GetValue<std::string>(14);
        bool is_available_in_mdx = stmt.GetValue<int>(15) != 0;
        int64_t dictionary_size = stmt.GetValue<int64_t>(16);
        std::string idf_filename = stmt.GetValue<std::string>(17);
        std::string encoding = stmt.GetValue<std::string>(18);
        int column_id = stmt.GetValue<int>(19);
        int table_id = stmt.GetValue<int>(20);

        std::string data_type = VpaxUtils::DataTypeIdToString(data_type_id);
        
        // Calculate actual sizes from file log
        int64_t data_size = GetFileSizeByName(idf_filename);
        int64_t total_size = dictionary_size + data_size;
        int64_t hierarchies_size = CalculateColumnHierarchySize(table_id, column_id);
        total_size += hierarchies_size;
        
        double selectivity = CalculateSelectivity(table_name, column_name);
        
        columns.push_back(VpaxValueFactory::CreateColumnValue(
            column_name, table_name, data_type, is_hidden, cardinality,
            total_size, dictionary_size, data_size, hierarchies_size, is_key, is_nullable,
            is_unique, keep_unique_rows, is_available_in_mdx, display_folder, encoding, description,
            expression, format_string, encoding_hint, state, selectivity
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
            m.Expression,
            m.isHidden,
            m.DataType,
            m.Description,
            m.FormatString,
            m.DisplayFolder,
            k.StatusExpression as KpiStatusExpression,
            k.TargetExpression as KpiTargetExpression,
            k.TargetFormatString as KpiTargetFormatString,
            k.TrendExpression as KpiTrendExpression,
            drd.Expression as DetailRowsExpression
        FROM Measure m
        JOIN [Table] t ON m.TableID = t.ID
        LEFT JOIN KPI k on m.KPIID = k.ID
        LEFT JOIN DetailRowsDefinition drd ON m.DetailRowsDefinitionID = drd.ID
        WHERE t.systemflags = 0
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
        std::string display_folder = stmt.GetValue<std::string>(7);
        std::string kpi_status = stmt.GetValue<std::string>(8);
        std::string kpi_target = stmt.GetValue<std::string>(9);
        std::string kpi_target_format = stmt.GetValue<std::string>(10);
        std::string kpi_trend = stmt.GetValue<std::string>(11);
        std::string detail_rows = stmt.GetValue<std::string>(12);
        
        std::string data_type = VpaxUtils::DataTypeIdToString(data_type_id);
        
        measures.push_back(VpaxValueFactory::CreateMeasureValue(
            measure_name, table_name, expression, is_hidden,
            data_type, description, format_string, display_folder,
            kpi_status, kpi_target, kpi_target_format, kpi_trend, detail_rows, false
        ));
    }
    
    return measures;
}

std::vector<Value> VpaxBuilder::BuildRelationships() {
    std::vector<Value> relationships;
    
    std::string sql = R"(
        SELECT 
            ft.Name AS FromTableName,
            fc.ExplicitName AS FromColumnName,
            tt.Name AS ToTableName,
            tc.ExplicitName AS ToColumnName,
            rel.IsActive,
            CASE 
                WHEN rel.FromCardinality = 2 THEN 'Many'
                ELSE 'One'
            END AS FromCardinalityType,
            CASE 
                WHEN rel.ToCardinality = 2 THEN 'Many'
                ELSE 'One'
            END AS ToCardinalityType,
            CASE 
                WHEN rel.CrossFilteringBehavior = 1 THEN 'Single'
                WHEN rel.CrossFilteringBehavior = 2 THEN 'Both'
                ELSE CAST(rel.CrossFilteringBehavior AS TEXT)
            END AS CrossFilteringBehavior,
            rid.RecordCount as UsedSizeFrom,
            rid2.RecordCount AS UsedSizeTo,
            rel.RelyOnReferentialIntegrity,
            rel.JoinOnDateBehavior,
            rel.Type,
            rel.SecurityFilteringBehavior,
            rel.FromCardinality,
            rel.ToCardinality
        FROM Relationship rel
            LEFT JOIN [Table] ft ON rel.FromTableID = ft.id
            LEFT JOIN [Column] fc ON rel.FromColumnID = fc.id
            LEFT JOIN [Table] tt ON rel.ToTableID = tt.id AND tt.systemflags = 0
            LEFT JOIN [Column] tc ON rel.ToColumnID = tc.id
            LEFT JOIN RelationshipStorage rs ON rs.id = rel.RelationshipStorageID
            LEFT JOIN RelationshipIndexStorage rid ON rs.RelationshipIndexStorageID = rid.id
            LEFT JOIN RelationshipStorage rs2 ON rs2.id = rel.RelationshipStorage2ID
            LEFT JOIN RelationshipIndexStorage rid2 ON rs2.RelationshipIndexStorageID = rid2.id
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string from_table = stmt.GetValue<std::string>(0);
        std::string from_column = stmt.GetValue<std::string>(1);
        std::string to_table = stmt.GetValue<std::string>(2);
        std::string to_column = stmt.GetValue<std::string>(3);
        bool is_active = stmt.GetValue<int>(4) != 0;
        std::string from_cardinality_type = stmt.GetValue<std::string>(5);
        std::string to_cardinality_type = stmt.GetValue<std::string>(6);
        std::string cross_filtering = stmt.GetValue<std::string>(7);
        int64_t used_size_from = stmt.GetValue<int64_t>(8);
        int64_t used_size_to = stmt.GetValue<int64_t>(9);
        bool rely_on_ri = stmt.GetValue<int>(10) != 0;
        int join_on_date_behavior = stmt.GetValue<int>(11);
        int relationship_type = stmt.GetValue<int>(12);
        int security_filtering_behavior = stmt.GetValue<int>(13);
        int64_t from_cardinality = stmt.GetValue<int64_t>(14);
        int64_t to_cardinality = stmt.GetValue<int64_t>(15);
        
        // Convert join on date behavior
        std::string join_behavior = "DateAndTime";
        if (join_on_date_behavior == 1) {
            join_behavior = "DatePartOnly";
        }
        
        // Convert relationship type
        std::string rel_type = "Regular";
        if (relationship_type == 1) {
            rel_type = "Limited";
        }
        
        // Convert security filtering behavior
        std::string security_behavior = "OneDirection";
        if (security_filtering_behavior == 2) {
            security_behavior = "BothDirections";
        }
        
        // Map CrossFilteringBehavior to match expected values
        if (cross_filtering == "Single") {
            cross_filtering = "OneDirection";
        } else if (cross_filtering == "Both") {
            cross_filtering = "BothDirections";
        }
        
        relationships.push_back(VpaxValueFactory::CreateRelationshipValue(
            from_table, from_column, to_table, to_column, is_active,
            from_cardinality, to_cardinality, from_cardinality_type, to_cardinality_type,
            cross_filtering, rel_type, rely_on_ri, used_size_from, used_size_to,
            used_size_from + used_size_to, 0, 0, 0.0, join_behavior, security_behavior
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
    std::vector<Value> hierarchies;

    std::string sql = R"(
        SELECT DISTINCT  
            t.Name as TableName,
            c.ExplicitName as ColumnName, 
            sc.ExplicitName as StructureName,
            sfi.FileName
        FROM COLUMN c
        JOIN [Table] t 
            ON c.TableId = t.ID
        JOIN AttributeHierarchy ah 
            ON ah.ColumnID = c.ID
        JOIN AttributeHierarchyStorage ahs 
            ON ah.AttributeHierarchyStorageID = ahs.ID
        join Column sc 
            on ahs.SystemTableID = sc.TableID
        JOIN ColumnPartitionStorage cps 
            ON cps.ColumnStorageID = sc.ColumnStorageID
        JOIN StorageFile sfi ON sfi.ID = cps.StorageFileID
    )";

    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string table_name = stmt.GetValue<std::string>(0);
        std::string column_name = stmt.GetValue<std::string>(1);
        std::string structure_name = stmt.GetValue<std::string>(2);
        std::string hidx_filename = stmt.GetValue<std::string>(3);
        int64_t hierarchy_size = GetFileSizeByName(hidx_filename);
        hierarchies.push_back(VpaxValueFactory::CreateColumnHierarchyValue(
            table_name, column_name, structure_name, hierarchy_size
        ));
    }

    return hierarchies;
}

std::vector<Value> VpaxBuilder::BuildUserHierarchies() {
    std::vector<Value> hierarchies;
    
    std::string sql = R"(
        SELECT 
            t.Name as TableName,
            h.Name as HierarchyName,
            h.isHidden,
            (SELECT GROUP_CONCAT(l.Name, '|')
            FROM Level l 
            WHERE l.HierarchyID = h.id
            ORDER BY l.Ordinal) as Levels,
            h.id
        FROM Hierarchy h
        JOIN [Table] t ON h.TableID = t.ID;
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string table_name = stmt.GetValue<std::string>(0);
        std::string hierarchy_name = stmt.GetValue<std::string>(1);
        bool is_hidden = stmt.GetValue<int>(2) != 0;
        std::string levels = stmt.GetValue<std::string>(3);
        int64_t hierarchy_id = stmt.GetValue<int64_t>(4);
        int64_t hierarchy_size = CalculateUserHierarchySize(hierarchy_id);

        hierarchies.push_back(VpaxValueFactory::CreateUserHierarchyValue(
            table_name, hierarchy_name, is_hidden, hierarchy_size, levels
        ));
    }
    
    return hierarchies;
}

std::vector<Value> VpaxBuilder::BuildPartitions() {
    std::vector<Value> partitions;
    
    std::string sql = R"(
        SELECT 
            t.Name as TableName, 
            p.Name as PartitionName, 
            p.QueryDefinition,
            sms.SegmentCount, 
            sms.RecordCount, 
            sms.RecordsPerSegment,
            p.RefreshedTime, 
            p.RefreshBookmark
        FROM SegmentMapStorage sms
        JOIN PartitionStorage ps ON ps.id = sms.PartitionStorageID
        JOIN Partition p ON p.ID = ps.PartitionID
        JOIN [Table] t ON t.ID = p.TableID
        WHERE p.SystemFlags = 0
    )";
    
    SQLiteStatement stmt = db_.Prepare(sql);
    while (stmt.Step()) {
        std::string table_name = stmt.GetValue<std::string>(0);
        std::string partition_name = stmt.GetValue<std::string>(1);
        std::string query_definition = stmt.GetValue<std::string>(2);
        int64_t segment_count = stmt.GetValue<int64_t>(3);
        int64_t record_count = stmt.GetValue<int64_t>(4);
        int64_t records_per_segment = stmt.GetValue<int64_t>(5);
        int64_t refreshed_time_ticks = stmt.GetValue<int64_t>(6);
        std::string refresh_bookmark = stmt.GetValue<std::string>(7);
        
        // Convert Windows file time to ISO 8601 string
        std::string refreshed_time = VpaxUtils::WindowsFileTimeToISO8601(refreshed_time_ticks);
        
        partitions.push_back(VpaxValueFactory::CreatePartitionValue(
            table_name,
            partition_name,
            query_definition,
            segment_count,
            record_count,
            records_per_segment,
            refreshed_time,
            refresh_bookmark
        ));
    }
    
    return partitions;
}

std::vector<Value> VpaxBuilder::BuildTablePermissions() {
    // Empty for now - would require security metadata
    return std::vector<Value>();
}

std::vector<Value> VpaxBuilder::BuildCalculationItems() {
    // Empty for now - would require calculation group metadata  
    return std::vector<Value>();
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
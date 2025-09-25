#include "vpax_schema.hpp"
#include "duckdb.hpp"

using namespace duckdb;

LogicalType VpaxSchema::CreateTableType() {
    child_list_t<LogicalType> table_children;
    table_children.push_back(make_pair("TableName", LogicalType::VARCHAR));
    table_children.push_back(make_pair("TableExpression", LogicalType::VARCHAR)); // Can be null
    table_children.push_back(make_pair("RowsCount", LogicalType::BIGINT));
    table_children.push_back(make_pair("ReferentialIntegrityViolationCount", LogicalType::BIGINT));
    table_children.push_back(make_pair("IsHidden", LogicalType::BOOLEAN));
    table_children.push_back(make_pair("IsPrivate", LogicalType::BOOLEAN));
    table_children.push_back(make_pair("IsLocalDateTable", LogicalType::BOOLEAN));
    table_children.push_back(make_pair("IsTemplateDateTable", LogicalType::BOOLEAN));
    table_children.push_back(make_pair("Description", LogicalType::VARCHAR));
    table_children.push_back(make_pair("ColumnsSize", LogicalType::BIGINT));
    table_children.push_back(make_pair("TableSize", LogicalType::BIGINT));
    table_children.push_back(make_pair("RelationshipsSize", LogicalType::BIGINT));
    table_children.push_back(make_pair("UserHierarchiesSize", LogicalType::BIGINT));
    table_children.push_back(make_pair("IsReferenced", LogicalType::BOOLEAN));
    
    return LogicalType::STRUCT(table_children);
}

LogicalType VpaxSchema::CreateColumnType() {
    child_list_t<LogicalType> column_children;
    column_children.push_back(make_pair("ColumnName", LogicalType::VARCHAR));
    column_children.push_back(make_pair("TableName", LogicalType::VARCHAR));
    column_children.push_back(make_pair("FullColumnName", LogicalType::VARCHAR));
    column_children.push_back(make_pair("ColumnCardinality", LogicalType::BIGINT));
    column_children.push_back(make_pair("DataType", LogicalType::VARCHAR));
    column_children.push_back(make_pair("ColumnType", LogicalType::VARCHAR));
    column_children.push_back(make_pair("IsHidden", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("Encoding", LogicalType::VARCHAR));
    column_children.push_back(make_pair("ColumnExpression", LogicalType::VARCHAR)); // Can be null
    column_children.push_back(make_pair("DisplayFolder", LogicalType::VARCHAR));
    column_children.push_back(make_pair("Description", LogicalType::VARCHAR));
    column_children.push_back(make_pair("FormatString", LogicalType::VARCHAR));
    column_children.push_back(make_pair("EncodingHint", LogicalType::VARCHAR));
    column_children.push_back(make_pair("IsAvailableInMDX", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("IsKey", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("IsNullable", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("IsUnique", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("KeepUniqueRows", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("SortByColumnName", LogicalType::VARCHAR));
    column_children.push_back(make_pair("State", LogicalType::VARCHAR));
    column_children.push_back(make_pair("IsRowNumber", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("IsReferenced", LogicalType::BOOLEAN));
    column_children.push_back(make_pair("DictionarySize", LogicalType::BIGINT));
    column_children.push_back(make_pair("DataSize", LogicalType::BIGINT));
    column_children.push_back(make_pair("HierarchiesSize", LogicalType::BIGINT));
    column_children.push_back(make_pair("TotalSize", LogicalType::BIGINT));
    column_children.push_back(make_pair("Selectivity", LogicalType::DOUBLE));
    
    return LogicalType::STRUCT(column_children);
}

LogicalType VpaxSchema::CreateMeasureType() {
    child_list_t<LogicalType> measure_children;
    measure_children.push_back(make_pair("MeasureName", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("TableName", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("FullMeasureName", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("MeasureExpression", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("DisplayFolder", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("Description", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("IsHidden", LogicalType::BOOLEAN));
    measure_children.push_back(make_pair("DataType", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("DetailRowsExpression", LogicalType::VARCHAR)); // Can be null
    measure_children.push_back(make_pair("FormatString", LogicalType::VARCHAR));
    measure_children.push_back(make_pair("KpiStatusExpression", LogicalType::VARCHAR)); // Can be null
    measure_children.push_back(make_pair("KpiTargetExpression", LogicalType::VARCHAR)); // Can be null
    measure_children.push_back(make_pair("KpiTargetFormatString", LogicalType::VARCHAR)); // Can be null
    measure_children.push_back(make_pair("KpiTrendExpression", LogicalType::VARCHAR)); // Can be null
    measure_children.push_back(make_pair("IsReferenced", LogicalType::BOOLEAN));
    
    return LogicalType::STRUCT(measure_children);
}

LogicalType VpaxSchema::CreateRelationshipType() {
    child_list_t<LogicalType> relationship_children;
    relationship_children.push_back(make_pair("FromTableName", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("FromFullColumnName", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("FromCardinality", LogicalType::BIGINT));
    relationship_children.push_back(make_pair("FromCardinalityType", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("ToTableName", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("ToFullColumnName", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("ToCardinality", LogicalType::BIGINT));
    relationship_children.push_back(make_pair("ToCardinalityType", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("RelyOnReferentialIntegrity", LogicalType::BOOLEAN));
    relationship_children.push_back(make_pair("JoinOnDateBehavior", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("CrossFilteringBehavior", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("RelationshipType", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("IsActive", LogicalType::BOOLEAN));
    relationship_children.push_back(make_pair("RelationshipName", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("SecurityFilteringBehavior", LogicalType::VARCHAR));
    relationship_children.push_back(make_pair("UsedSizeFrom", LogicalType::BIGINT));
    relationship_children.push_back(make_pair("UsedSizeTo", LogicalType::BIGINT));
    relationship_children.push_back(make_pair("UsedSize", LogicalType::BIGINT));
    relationship_children.push_back(make_pair("MissingKeys", LogicalType::BIGINT));
    relationship_children.push_back(make_pair("InvalidRows", LogicalType::BIGINT));
    relationship_children.push_back(make_pair("OneToManyRatio", LogicalType::DOUBLE));
    
    return LogicalType::STRUCT(relationship_children);
}

LogicalType VpaxSchema::CreateColumnSegmentType() {
    child_list_t<LogicalType> segment_children;
    segment_children.push_back(make_pair("ColumnName", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("TableName", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("FullColumnName", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("PartitionName", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("PartitionState", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("PartitionType", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("PartitionMode", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("RefreshedTime", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("SegmentNumber", LogicalType::INTEGER));
    segment_children.push_back(make_pair("TablePartitionNumber", LogicalType::INTEGER));
    segment_children.push_back(make_pair("SegmentRows", LogicalType::BIGINT));
    segment_children.push_back(make_pair("UsedSize", LogicalType::BIGINT));
    segment_children.push_back(make_pair("CompressionType", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("BitsCount", LogicalType::INTEGER));
    segment_children.push_back(make_pair("BookmarkBitsCount", LogicalType::INTEGER));
    segment_children.push_back(make_pair("VertipaqState", LogicalType::VARCHAR));
    segment_children.push_back(make_pair("IsPageable", LogicalType::BOOLEAN)); // Can be null
    segment_children.push_back(make_pair("IsResident", LogicalType::BOOLEAN)); // Can be null
    segment_children.push_back(make_pair("Temperature", LogicalType::VARCHAR)); // Can be null
    segment_children.push_back(make_pair("LastAccessed", LogicalType::VARCHAR)); // Can be null
    
    return LogicalType::STRUCT(segment_children);
}

LogicalType VpaxSchema::CreateUserHierarchyType() {
    child_list_t<LogicalType> hierarchy_children;
    hierarchy_children.push_back(make_pair("TableName", LogicalType::VARCHAR));
    hierarchy_children.push_back(make_pair("UserHierarchyName", LogicalType::VARCHAR));
    hierarchy_children.push_back(make_pair("IsHidden", LogicalType::BOOLEAN));
    hierarchy_children.push_back(make_pair("UsedSize", LogicalType::BIGINT));
    hierarchy_children.push_back(make_pair("Levels", LogicalType::VARCHAR));
    
    return LogicalType::STRUCT(hierarchy_children);
}

LogicalType VpaxSchema::CreateColumnHierarchyType() {
    child_list_t<LogicalType> hierarchy_children;
    hierarchy_children.push_back(make_pair("ColumnName", LogicalType::VARCHAR));
    hierarchy_children.push_back(make_pair("TableName", LogicalType::VARCHAR));
    hierarchy_children.push_back(make_pair("FullColumnName", LogicalType::VARCHAR));
    hierarchy_children.push_back(make_pair("StructureName", LogicalType::VARCHAR));
    hierarchy_children.push_back(make_pair("SegmentNumber", LogicalType::INTEGER));
    hierarchy_children.push_back(make_pair("TablePartitionNumber", LogicalType::INTEGER));
    hierarchy_children.push_back(make_pair("UsedSize", LogicalType::BIGINT));
    
    return LogicalType::STRUCT(hierarchy_children);
}

LogicalType VpaxSchema::CreateErrorType() {
    child_list_t<LogicalType> error_children;
    error_children.push_back(make_pair("error", LogicalType::VARCHAR));
    error_children.push_back(make_pair("message", LogicalType::VARCHAR));
    error_children.push_back(make_pair("file", LogicalType::VARCHAR));
    error_children.push_back(make_pair("section", LogicalType::VARCHAR));
    
    return LogicalType::STRUCT(error_children);
}

LogicalType VpaxSchema::CreateVpaxType() {
    child_list_t<LogicalType> vpax_children;
    
    // Add all VPAX sections
    vpax_children.push_back(make_pair("Tables", LogicalType::LIST(CreateTableType())));
    vpax_children.push_back(make_pair("Columns", LogicalType::LIST(CreateColumnType())));
    vpax_children.push_back(make_pair("Measures", LogicalType::LIST(CreateMeasureType())));
    vpax_children.push_back(make_pair("ColumnsSegments", LogicalType::LIST(CreateColumnSegmentType())));
    vpax_children.push_back(make_pair("ColumnsHierarchies", LogicalType::LIST(CreateColumnHierarchyType())));
    vpax_children.push_back(make_pair("UserHierarchies", LogicalType::LIST(CreateUserHierarchyType())));
    vpax_children.push_back(make_pair("Relationships", LogicalType::LIST(CreateRelationshipType())));
    vpax_children.push_back(make_pair("TablePermissions", LogicalType::LIST(LogicalType::VARCHAR))); // Empty arrays
    vpax_children.push_back(make_pair("CalculationItems", LogicalType::LIST(LogicalType::VARCHAR))); // Empty arrays
    
    return LogicalType::STRUCT(vpax_children);
}
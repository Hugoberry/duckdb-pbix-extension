#include "vpax_schema.hpp"
#include "duckdb.hpp"

using namespace duckdb;

Value VpaxValueFactory::CreateTableValue(
    const std::string &table_name,
    int64_t row_count,
    bool is_hidden,
    bool is_private,
    int64_t columns_size,
    int64_t table_size,
    int64_t relationships_size,
    int64_t hierarchies_size,
    bool is_referenced,
    int64_t ri_violation_count,
    const std::string &description) {
    
    child_list_t<Value> table_values;
    table_values.push_back(make_pair("TableName", Value(table_name)));
    table_values.push_back(make_pair("TableExpression", Value())); // NULL
    table_values.push_back(make_pair("RowsCount", Value::BIGINT(row_count)));
    table_values.push_back(make_pair("ReferentialIntegrityViolationCount", Value::BIGINT(ri_violation_count)));
    table_values.push_back(make_pair("IsHidden", Value::BOOLEAN(is_hidden)));
    table_values.push_back(make_pair("IsPrivate", Value::BOOLEAN(is_private)));
    table_values.push_back(make_pair("IsLocalDateTable", Value::BOOLEAN(false)));
    table_values.push_back(make_pair("IsTemplateDateTable", Value::BOOLEAN(false)));
    table_values.push_back(make_pair("Description", Value(description)));
    table_values.push_back(make_pair("ColumnsSize", Value::BIGINT(columns_size)));
    table_values.push_back(make_pair("TableSize", Value::BIGINT(table_size)));
    table_values.push_back(make_pair("RelationshipsSize", Value::BIGINT(relationships_size)));
    table_values.push_back(make_pair("UserHierarchiesSize", Value::BIGINT(hierarchies_size)));
    table_values.push_back(make_pair("IsReferenced", Value::BOOLEAN(is_referenced)));
    
    return Value::STRUCT(table_values);
}

Value VpaxValueFactory::CreateColumnValue(
    const std::string &column_name,
    const std::string &table_name,
    const std::string &data_type,
    bool is_hidden,
    int64_t cardinality,
    int64_t total_size,
    int64_t dictionary_size,
    int64_t data_size,
    int64_t hier_size,
    bool is_key,
    bool is_nullable,
    bool is_unique,
    bool keep_unique_rows,
    bool is_available_in_mdx,
    const std::string &display_folder,
    const std::string &encoding,
    const std::string &description,
    const std::string &expression,
    const std::string &format_string,
    const std::string &encoding_hint,
    const std::string &state,
    double selectivity) {
    
    child_list_t<Value> column_values;
    column_values.push_back(make_pair("ColumnName", Value(column_name)));
    column_values.push_back(make_pair("TableName", Value(table_name)));
    column_values.push_back(make_pair("FullColumnName", Value(table_name + "[" + column_name + "]")));
    column_values.push_back(make_pair("ColumnCardinality", Value::BIGINT(cardinality)));
    column_values.push_back(make_pair("DataType", Value(data_type)));
    column_values.push_back(make_pair("ColumnType", Value("Data")));
    column_values.push_back(make_pair("IsHidden", Value::BOOLEAN(is_hidden)));
    column_values.push_back(make_pair("Encoding", Value(encoding)));
    column_values.push_back(make_pair("ColumnExpression", Value(expression)));
    column_values.push_back(make_pair("DisplayFolder", Value(display_folder)));
    column_values.push_back(make_pair("Description", Value(description)));
    column_values.push_back(make_pair("FormatString", Value(format_string)));
    column_values.push_back(make_pair("EncodingHint", Value(encoding_hint)));
    column_values.push_back(make_pair("IsAvailableInMDX", Value::BOOLEAN(is_available_in_mdx)));
    column_values.push_back(make_pair("IsKey", Value::BOOLEAN(is_key)));
    column_values.push_back(make_pair("IsNullable", Value::BOOLEAN(is_nullable)));
    column_values.push_back(make_pair("IsUnique", Value::BOOLEAN(is_unique)));
    column_values.push_back(make_pair("KeepUniqueRows", Value::BOOLEAN(keep_unique_rows)));
    column_values.push_back(make_pair("SortByColumnName", Value("")));
    column_values.push_back(make_pair("State", Value(state)));
    column_values.push_back(make_pair("IsRowNumber", Value::BOOLEAN(false)));
    column_values.push_back(make_pair("IsReferenced", Value::BOOLEAN(false))); // TODO: Implement
    column_values.push_back(make_pair("DictionarySize", Value::BIGINT(dictionary_size)));
    column_values.push_back(make_pair("DataSize", Value::BIGINT(data_size)));
    column_values.push_back(make_pair("HierarchiesSize", Value::BIGINT(hier_size)));
    column_values.push_back(make_pair("TotalSize", Value::BIGINT(total_size)));
    column_values.push_back(make_pair("Selectivity", Value::DOUBLE(selectivity)));
    
    return Value::STRUCT(column_values);
}

Value VpaxValueFactory::CreateMeasureValue(
    const std::string &measure_name,
    const std::string &table_name,
    const std::string &expression,
    bool is_hidden,
    const std::string &data_type,
    const std::string &description,
    const std::string &format_string,
    const std::string &display_folder,
    const std::string &kpi_status,
    const std::string &kpi_target,
    const std::string &kpi_target_format,
    const std::string &kpi_trend,
    const std::string &detail_rows,
    bool is_referenced) {
    
    child_list_t<Value> measure_values;
    measure_values.push_back(make_pair("MeasureName", Value(measure_name)));
    measure_values.push_back(make_pair("TableName", Value(table_name)));
    measure_values.push_back(make_pair("FullMeasureName", Value("[" + table_name + "].[" + measure_name + "]")));
    measure_values.push_back(make_pair("MeasureExpression", Value(expression)));
    measure_values.push_back(make_pair("DisplayFolder", Value(display_folder)));
    measure_values.push_back(make_pair("Description", Value(description)));
    measure_values.push_back(make_pair("IsHidden", Value::BOOLEAN(is_hidden)));
    measure_values.push_back(make_pair("DataType", Value(data_type)));
    measure_values.push_back(make_pair("DetailRowsExpression", Value(detail_rows)));
    measure_values.push_back(make_pair("FormatString", Value(format_string)));
    measure_values.push_back(make_pair("KpiStatusExpression", Value(kpi_status)));
    measure_values.push_back(make_pair("KpiTargetExpression", Value(kpi_target)));
    measure_values.push_back(make_pair("KpiTargetFormatString", Value(kpi_target_format)));
    measure_values.push_back(make_pair("KpiTrendExpression", Value(kpi_trend)));
    measure_values.push_back(make_pair("IsReferenced", Value::BOOLEAN(is_referenced)));
    
    return Value::STRUCT(measure_values);
}

Value VpaxValueFactory::CreateRelationshipValue(
    const std::string &from_table,
    const std::string &from_column,
    const std::string &to_table,
    const std::string &to_column,
    bool is_active,
    int64_t from_cardinality,
    int64_t to_cardinality,
    const std::string &from_cardinality_type,
    const std::string &to_cardinality_type,
    const std::string &cross_filtering,
    const std::string &relationship_type,
    bool rely_on_ri,
    int64_t used_size_from,
    int64_t used_size_to,
    int64_t used_size,
    int64_t missing_keys,
    int64_t invalid_rows,
    double one_to_many_ratio,
    const std::string &join_on_date_behavior,
    const std::string &security_filtering_behavior) {
    
    child_list_t<Value> rel_values;
    rel_values.push_back(make_pair("FromTableName", Value(from_table)));
    rel_values.push_back(make_pair("FromFullColumnName", Value(from_table + "[" + from_column + "]")));
    rel_values.push_back(make_pair("FromCardinality", Value::BIGINT(from_cardinality)));
    rel_values.push_back(make_pair("FromCardinalityType", Value(from_cardinality_type)));
    rel_values.push_back(make_pair("ToTableName", Value(to_table)));
    rel_values.push_back(make_pair("ToFullColumnName", Value(to_table + "[" + to_column + "]")));
    rel_values.push_back(make_pair("ToCardinality", Value::BIGINT(to_cardinality)));
    rel_values.push_back(make_pair("ToCardinalityType", Value(to_cardinality_type)));
    rel_values.push_back(make_pair("RelyOnReferentialIntegrity", Value::BOOLEAN(rely_on_ri)));
    rel_values.push_back(make_pair("JoinOnDateBehavior", Value(join_on_date_behavior)));
    rel_values.push_back(make_pair("CrossFilteringBehavior", Value(cross_filtering)));
    rel_values.push_back(make_pair("RelationshipType", Value(relationship_type)));
    rel_values.push_back(make_pair("IsActive", Value::BOOLEAN(is_active)));
    rel_values.push_back(make_pair("RelationshipName", Value("")));
    rel_values.push_back(make_pair("SecurityFilteringBehavior", Value(security_filtering_behavior)));
    rel_values.push_back(make_pair("UsedSizeFrom", Value::BIGINT(used_size_from)));
    rel_values.push_back(make_pair("UsedSizeTo", Value::BIGINT(used_size_to)));
    rel_values.push_back(make_pair("UsedSize", Value::BIGINT(used_size)));
    rel_values.push_back(make_pair("MissingKeys", Value::BIGINT(missing_keys)));
    rel_values.push_back(make_pair("InvalidRows", Value::BIGINT(invalid_rows)));
    rel_values.push_back(make_pair("OneToManyRatio", Value::DOUBLE(one_to_many_ratio)));
    
    return Value::STRUCT(rel_values);
}

Value VpaxValueFactory::CreateColumnSegmentValue(
    const std::string &column_name,
    const std::string &table_name,
    const std::string &partition_name,
    int segment_number,
    int table_partition_number,
    int64_t segment_rows,
    int64_t used_size,
    const std::string &compression_type,
    int bits_count,
    int bookmark_bits_count,
    const std::string &vertipaq_state) {
    
    child_list_t<Value> segment_values;
    segment_values.push_back(make_pair("ColumnName", Value(column_name)));
    segment_values.push_back(make_pair("TableName", Value(table_name)));
    segment_values.push_back(make_pair("FullColumnName", Value(table_name + "[" + column_name + "]")));
    segment_values.push_back(make_pair("PartitionName", Value(partition_name)));
    segment_values.push_back(make_pair("PartitionState", Value("Ready")));
    segment_values.push_back(make_pair("PartitionType", Value("Data")));
    segment_values.push_back(make_pair("PartitionMode", Value("Import")));
    segment_values.push_back(make_pair("RefreshedTime", Value("")));
    segment_values.push_back(make_pair("SegmentNumber", Value::INTEGER(segment_number)));
    segment_values.push_back(make_pair("TablePartitionNumber", Value::INTEGER(table_partition_number)));
    segment_values.push_back(make_pair("SegmentRows", Value::BIGINT(segment_rows)));
    segment_values.push_back(make_pair("UsedSize", Value::BIGINT(used_size)));
    segment_values.push_back(make_pair("CompressionType", Value(compression_type)));
    segment_values.push_back(make_pair("BitsCount", Value::INTEGER(bits_count)));
    segment_values.push_back(make_pair("BookmarkBitsCount", Value::INTEGER(bookmark_bits_count)));
    segment_values.push_back(make_pair("VertipaqState", Value(vertipaq_state)));
    segment_values.push_back(make_pair("IsPageable", Value())); // NULL
    segment_values.push_back(make_pair("IsResident", Value())); // NULL
    segment_values.push_back(make_pair("Temperature", Value())); // NULL
    segment_values.push_back(make_pair("LastAccessed", Value())); // NULL
    
    return Value::STRUCT(segment_values);
}

Value VpaxValueFactory::CreateUserHierarchyValue(
    const std::string &table_name,
    const std::string &hierarchy_name,
    bool is_hidden,
    int64_t used_size,
    const std::string &levels) {
    
    child_list_t<Value> hierarchy_values;
    hierarchy_values.push_back(make_pair("TableName", Value(table_name)));
    hierarchy_values.push_back(make_pair("UserHierarchyName", Value(hierarchy_name)));
    hierarchy_values.push_back(make_pair("IsHidden", Value::BOOLEAN(is_hidden)));
    hierarchy_values.push_back(make_pair("UsedSize", Value::BIGINT(used_size)));
    hierarchy_values.push_back(make_pair("Levels", Value(levels)));
    
    return Value::STRUCT(hierarchy_values);
}

Value VpaxValueFactory::CreateColumnHierarchyValue(
    const std::string &table_name,
    const std::string &column_name,
    const std::string &structure_name,
    int64_t hierarchy_size) {
    
    child_list_t<Value> hierarchy_values;
    hierarchy_values.push_back(make_pair("TableName", Value(table_name)));
    hierarchy_values.push_back(make_pair("ColumnName", Value(column_name)));
    hierarchy_values.push_back(make_pair("FullColumnName", Value(table_name + "[" + column_name + "]")));
    hierarchy_values.push_back(make_pair("StructureName", Value(structure_name)));
    hierarchy_values.push_back(make_pair("UsedSize", Value::BIGINT(hierarchy_size)));
    
    return Value::STRUCT(hierarchy_values);
}

Value VpaxValueFactory::CreateErrorValue(
    const std::string &message,
    const std::string &file_name,
    const std::string &section) {
    
    child_list_t<Value> error_values;
    error_values.push_back(make_pair("error", Value("VPAX Generation Failed")));
    error_values.push_back(make_pair("message", Value(message)));
    error_values.push_back(make_pair("file", Value(file_name)));
    error_values.push_back(make_pair("section", Value(section)));
    
    return Value::STRUCT(error_values);
}
#pragma once

#include "duckdb.hpp"

namespace duckdb {

// VPAX Schema Type Definitions
class VpaxSchema {
public:
    // Main VPAX structure type
    static LogicalType CreateVpaxType();
    
    // Individual section types
    static LogicalType CreateTableType();
    static LogicalType CreateColumnType();
    static LogicalType CreateMeasureType();
    static LogicalType CreateRelationshipType();
    static LogicalType CreateColumnSegmentType();
    static LogicalType CreateColumnHierarchyType();
    static LogicalType CreateUserHierarchyType();
    static LogicalType CreatePartitionType();
    static LogicalType CreateTablePermissionType();
    
    // Helper types
    static LogicalType CreateErrorType();
    static LogicalType CreateMetadataType();
};

// VPAX Value Creation Helpers
class VpaxValueFactory {
public:
    // Create individual struct values
    static Value CreateTableValue(
        const std::string &table_name,
        int64_t row_count,
        bool is_hidden,
        bool is_private = false,
        int64_t columns_size = 0,
        int64_t table_size = 0,
        int64_t relationships_size = 0,
        int64_t hierarchies_size = 0,
        bool is_referenced = false,
        int64_t ri_violation_count = 0,
        const std::string &description = ""
    );
    
    static Value CreateColumnValue(
        const std::string &column_name,
        const std::string &table_name,
        const std::string &data_type,
        bool is_hidden = false,
        int64_t cardinality = 0,
        int64_t total_size = 0,
        int64_t dictionary_size = 0,
        int64_t data_size = 0,
        int64_t hier_size = 0,
        bool is_key = false,
        bool is_nullable = true,
        bool is_unique = false,
        bool keep_unique_rows = false,
        bool is_available_in_mdx = true,
        const std::string &display_folder = "",
        const std::string &encoding = "Hash",
        const std::string &description = "",
        const std::string &expression = "",
        const std::string &format_string = "",
        const std::string &encoding_hint = "",
        const std::string &state = "",
        double selectivity = 0.0
    );
    
    static Value CreateMeasureValue(
        const std::string &measure_name,
        const std::string &table_name,
        const std::string &expression,
        bool is_hidden = false,
        const std::string &data_type = "Variant",
        const std::string &description = "",
        const std::string &format_string = "",
        const std::string &display_folder = "",
        const std::string &kpi_status = "",
        const std::string &kpi_target = "",
        const std::string &kpi_target_format = "",
        const std::string &kpi_trend = "",
        const std::string &detail_rows = "",
        bool is_referenced = false
    );
    
    static Value CreateRelationshipValue(
        const std::string &from_table,
        const std::string &from_column,
        const std::string &to_table,
        const std::string &to_column,
        bool is_active = true,
        int64_t from_cardinality = 0,
        int64_t to_cardinality = 0,
        const std::string &from_cardinality_type = "Many",
        const std::string &to_cardinality_type = "One",
        const std::string &cross_filtering = "OneDirection",
        const std::string &relationship_type = "Regular",
        bool rely_on_ri = false,
        int64_t used_size_from = 0,
        int64_t used_size_to = 0,
        int64_t used_size = 0,
        int64_t missing_keys = 0,
        int64_t invalid_rows = 0,
        double one_to_many_ratio = 0.0,
        const std::string &join_on_date_behavior = "DateAndTime",
        const std::string &security_filtering_behavior = "OneDirection"
    );
    
    static Value CreateColumnSegmentValue(
        const std::string &column_name,
        const std::string &table_name,
        const std::string &partition_name,
        int segment_number = 0,
        int table_partition_number = 0,
        int64_t segment_rows = 0,
        int64_t used_size = 0,
        const std::string &compression_type = "Unknown",
        int bits_count = 0,
        int bookmark_bits_count = 0,
        const std::string &vertipaq_state = "Ready"
    );
    
    static Value CreateUserHierarchyValue(
        const std::string &table_name,
        const std::string &hierarchy_name,
        bool is_hidden = false,
        int64_t used_size = 0,
        const std::string &levels = ""
    );

    static Value CreateColumnHierarchyValue(
        const std::string &column_name,
        const std::string &table_name,
        const std::string &structure_name,
        int64_t used_size = 0
    );
    
    static Value CreatePartitionValue(
        const std::string &table_name,
        const std::string &partition_name,
        const std::string &query_definition = "",
        int64_t segment_count = 0,
        int64_t record_count = 0,
        int64_t records_per_segment = 0,
        const std::string &refreshed_time = "",
        const std::string &refresh_bookmark = ""
    );
    
    static Value CreateTablePermissionValue(
        const std::string &role_name,
        const std::string &table_name,
        const std::string &filter_expression
    );
    
    // Error value
    static Value CreateErrorValue(
        const std::string &message,
        const std::string &file_name,
        const std::string &section = ""
    );
};

} // namespace duckdb
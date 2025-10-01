#pragma once

#include "duckdb.hpp"
#include <string>

namespace duckdb {

// VPAX-specific enums and constants
enum class VpaxDataType {
    STRING = 2,
    INT64 = 6, 
    DOUBLE = 8,
    DATETIME = 9,
    CURRENCY = 10,
    BOOLEAN = 11,
    BINARY = 17,
    VARIANT = 0
};

enum class VpaxColumnType {
    DATA,
    CALCULATED,
    ROWKEY,
    CALCULATED_TABLE_COLUMN
};

enum class VpaxEncodingType {
    HASH,
    VALUE,
    RLE
};

// VPAX utility functions
class VpaxUtils {
public:
    static std::string DataTypeIdToString(int type_id);
    static std::string ColumnTypeToString(VpaxColumnType column_type);
    static std::string EncodingTypeToString(VpaxEncodingType encoding_type);
    static VpaxDataType IntToDataType(int type_id);
    static bool CheckIfTableIsReferenced(class SQLiteDB &db, const std::string &table_name);
    static bool CheckIfColumnIsReferenced(class SQLiteDB &db, const std::string &table_name, const std::string &column_name);
    static std::string WindowsFileTimeToISO8601(int64_t windows_ticks);
};

// Error handling
struct VpaxError {
    std::string message;
    std::string file_name;
    std::string section;
};

} // namespace duckdb
#include "vpax_types.hpp"
#include "sqlite_db.hpp"
#include "sqlite_stmt.hpp"

using namespace duckdb;

std::string VpaxUtils::DataTypeIdToString(int type_id) {
    switch (type_id) {
        case static_cast<int>(VpaxDataType::STRING): return "String";
        case static_cast<int>(VpaxDataType::INT64): return "Int64";
        case static_cast<int>(VpaxDataType::DOUBLE): return "Double";
        case static_cast<int>(VpaxDataType::DATETIME): return "DateTime";
        case static_cast<int>(VpaxDataType::CURRENCY): return "Currency";
        case static_cast<int>(VpaxDataType::BOOLEAN): return "Boolean";
        case static_cast<int>(VpaxDataType::BINARY): return "Binary";
        default: return "Variant";
    }
}

std::string VpaxUtils::ColumnTypeToString(VpaxColumnType column_type) {
    switch (column_type) {
        case VpaxColumnType::DATA: return "Data";
        case VpaxColumnType::CALCULATED: return "Calculated";
        case VpaxColumnType::ROWKEY: return "RowKey";
        case VpaxColumnType::CALCULATED_TABLE_COLUMN: return "CalculatedTableColumn";
        default: return "Data";
    }
}

std::string VpaxUtils::EncodingTypeToString(VpaxEncodingType encoding_type) {
    switch (encoding_type) {
        case VpaxEncodingType::HASH: return "Hash";
        case VpaxEncodingType::VALUE: return "Value";
        case VpaxEncodingType::RLE: return "RLE";
        default: return "Hash";
    }
}

VpaxDataType VpaxUtils::IntToDataType(int type_id) {
    switch (type_id) {
        case 2: return VpaxDataType::STRING;
        case 6: return VpaxDataType::INT64;
        case 8: return VpaxDataType::DOUBLE;
        case 9: return VpaxDataType::DATETIME;
        case 10: return VpaxDataType::CURRENCY;
        case 11: return VpaxDataType::BOOLEAN;
        case 17: return VpaxDataType::BINARY;
        default: return VpaxDataType::VARIANT;
    }
}

bool VpaxUtils::CheckIfTableIsReferenced(SQLiteDB &db, const std::string &table_name) {
    try {
        std::string ref_sql = R"(
            SELECT COUNT(*) 
            FROM Relationship r
            JOIN COLUMN ct ON r.ToColumnID = ct.ID
            JOIN [Table] tt ON ct.TableId = tt.ID
            WHERE tt.Name = ?
        )";
        
        SQLiteStatement ref_stmt = db.Prepare(ref_sql);
        // FIX: Use BindText for strings instead of Bind
        ref_stmt.BindText(0, string_t(table_name));
        
        if (ref_stmt.Step()) {
            return ref_stmt.GetValue<int>(0) > 0;
        }
    } catch (const std::exception &e) {
        // If query fails, assume not referenced
        return false;
    }
    
    return false;
}

std::string VpaxUtils::WindowsFileTimeToISO8601(int64_t windows_ticks) {
    if (windows_ticks <= 0) {
        return "";
    }
    
    // Windows epoch starts at 1601-01-01, Unix epoch at 1970-01-01
    // Windows file time is in 100-nanosecond intervals
    const int64_t WINDOWS_TICK = 10000000; // 100-nanosecond intervals per second
    const int64_t SEC_TO_UNIX_EPOCH = 11644473600LL; // Seconds between 1601 and 1970
    
    int64_t unix_seconds = (windows_ticks / WINDOWS_TICK) - SEC_TO_UNIX_EPOCH;
    
    // Convert to ISO 8601 format
    time_t time = static_cast<time_t>(unix_seconds);
    struct tm *tm_info = gmtime(&time);
    if (!tm_info) {
        return "";
    }
    
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", tm_info);
    return std::string(buffer);
}

bool VpaxUtils::CheckIfColumnIsReferenced(SQLiteDB &db, const std::string &table_name, const std::string &column_name) {
    try {
        std::string ref_sql = R"(
            SELECT COUNT(*) 
            FROM Relationship r
            JOIN COLUMN c ON (r.FromColumnID = c.ID OR r.ToColumnID = c.ID)
            JOIN [Table] t ON c.TableId = t.ID
            WHERE t.Name = ? AND c.ExplicitName = ?
        )";
        
        SQLiteStatement ref_stmt = db.Prepare(ref_sql);
        // FIX: Use BindText for strings instead of Bind
        ref_stmt.BindText(0, string_t(table_name));
        ref_stmt.BindText(1, string_t(column_name));
        
        if (ref_stmt.Step()) {
            return ref_stmt.GetValue<int>(0) > 0;
        }
    } catch (const std::exception &e) {
        // If query fails, assume not referenced
        return false;
    }
    
    return false;
}
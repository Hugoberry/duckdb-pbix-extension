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
        ref_stmt.Bind(0, table_name);
        
        if (ref_stmt.Step()) {
            return ref_stmt.GetValue<int>(0) > 0;
        }
    } catch (const std::exception &e) {
        // If query fails, assume not referenced
        return false;
    }
    
    return false;
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
        ref_stmt.Bind(0, table_name);
        ref_stmt.Bind(1, column_name);
        
        if (ref_stmt.Step()) {
            return ref_stmt.GetValue<int>(0) > 0;
        }
    } catch (const std::exception &e) {
        // If query fails, assume not referenced
        return false;
    }
    
    return false;
}
#include "AbfParser.h"
#include <fstream>
// tinyxml and sqlite3 linked through vcpkg


using namespace tinyxml2;

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}
std::vector<uint8_t> AbfParser::read_buffer_bytes(const std::vector<uint8_t>& buffer, long offset, int size) {
    std::vector<uint8_t> sub_buffer(size);
    std::copy(buffer.begin() + offset, buffer.begin() + offset + size, sub_buffer.begin());
    return sub_buffer;
}

std::vector<uint8_t> AbfParser::trim_buffer(const std::vector<uint8_t>& buffer) {
    int last_non_zero_byte = buffer.size() - 1;
    while (last_non_zero_byte >= 1 && buffer[last_non_zero_byte] == 0x00 && buffer[last_non_zero_byte - 1] == 0x00) {
        last_non_zero_byte -= 2;
    }
    last_non_zero_byte++;
    std::vector<uint8_t> trimmed_buffer(buffer.begin(), buffer.begin() + last_non_zero_byte + 1);
    return trimmed_buffer;
}

void AbfParser::process_data(const std::vector<uint8_t>& decompressed_buffer) {
    int offset = 72;
    int page = 0x1000;

    std::vector<uint8_t> backup_log_header_buffer = read_buffer_bytes(decompressed_buffer, offset, page - offset);
    backup_log_header_buffer = trim_buffer(backup_log_header_buffer);
    BackupLogHeader backup_log_header = BackupLogHeader::from_xml(backup_log_header_buffer, "UTF-16");

    int virtual_directory_offset = static_cast<int>(backup_log_header.m_cbOffsetHeader);
    int virtual_directory_size = static_cast<int>(backup_log_header.DataSize);
    std::vector<uint8_t> virtual_directory_buffer = read_buffer_bytes(decompressed_buffer, virtual_directory_offset, virtual_directory_size);
    VirtualDirectory virtual_directory = VirtualDirectory::from_xml(virtual_directory_buffer, "UTF-8");

    if (virtual_directory.backupFiles && !virtual_directory.backupFiles->empty()) {
        BackupFile log = virtual_directory.backupFiles->back();
        int backup_file_offset = static_cast<int>(log.m_cbOffsetHeader);
        int backup_file_size = static_cast<int>(log.Size);
        std::vector<uint8_t> backup_log_buffer = read_buffer_bytes(decompressed_buffer, backup_file_offset+2, backup_file_size-2);
        BackupLog backup_log = BackupLog::from_xml(backup_log_buffer, "UTF-16");

        auto sqlite = (*virtual_directory.backupFiles)[static_cast<int>(virtual_directory.backupFiles->size()) - 2];
        int sqlite_offset = static_cast<int>(sqlite.m_cbOffsetHeader);
        int sqlite_size = static_cast<int>(sqlite.Size);
        std::vector<uint8_t> sqlite_buffer = read_buffer_bytes(decompressed_buffer, sqlite_offset, sqlite_size);

        // Write sqlite_buffer to a temporary file
        std::ofstream temp_file("temp.db", std::ios::binary);
        temp_file.write(reinterpret_cast<const char*>(sqlite_buffer.data()), sqlite_buffer.size());

        // Open the temporary file with sqlite3_open
        sqlite3* db;
        char* zErrMsg = 0;
        if (sqlite3_open("temp.db", &db)) {
            std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
            return;
        }

        // Execute the query
        std::string sql = "SELECT name FROM sqlite_schema WHERE type ='table' AND name NOT LIKE 'sqlite_%'";
        if (sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg) != SQLITE_OK) {
            std::cerr << "SQL error: " << zErrMsg << std::endl;
            sqlite3_free(zErrMsg);
        }

        // Close the database
        sqlite3_close(db);

    }
}


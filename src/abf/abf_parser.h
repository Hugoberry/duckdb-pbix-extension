#pragma once

#include <cstdint>
#include <vector>
#include <tuple>
#include "backup_log_header.h"
#include "virtual_directory.h"
#include "backup_log.h"

#include <fstream>
#include <sstream>

#include "Xpress9Wrapper.h"
#include "crc32.h"
#include "duckdb.hpp"
#include "zip_utils.h"
#include "vertipaq_meta.hpp"

class AbfParser {
public:
    static DataModel get_sqlite(duckdb::ClientContext &context,const std::string &path, const int trailing_chunks);
private:
    // duckdb::FileHandle *file_handle;
    // mz_zip_archive zip_archive;
    static void patch_header_of_compressed_buffer(std::vector<uint8_t> &compressed_buffer, uint32_t& block_index_iterator);
    static std::vector<uint8_t> read_buffer_bytes(const std::vector<uint8_t>& buffer, uint64_t offset, int size);
    static std::vector<uint8_t> trim_buffer(const std::vector<uint8_t>& buffer);
    static BackupLogHeader process_backup_log_header(const std::vector<uint8_t> &buffer);
    static DataModel extract_sqlite_buffer(const std::vector<uint8_t> &buffer, uint64_t skip_offset, BackupLogHeader virtual_directory);
    static std::pair<uint64_t, uint64_t> locate_datamodel(duckdb::FileHandle &file_handle, const std::string &path);
    static void read_compressed_datamodel_header(std::ifstream &entryStream, uint64_t &datamodel_ofs);
    static std::vector<VertipaqFile> match_logs_and_get_vertipaq_meta(const BackupLog& backupLog, const VirtualDirectory& virtualDirectory);
    static std::vector<uint8_t> decompress_initial_block(duckdb::FileHandle &file_handle, uint64_t &bytes_read, XPress9Wrapper &xpress9_wrapper);
    static std::vector<uint8_t> iterate_and_decompress_blocks(duckdb::FileHandle &file_handle, uint64_t &bytes_read, uint64_t datamodel_ofs, uint64_t datamodel_size, XPress9Wrapper &xpress9_wrapper, BackupLogHeader virtual_directory, const int trailing_blocks, uint64_t &skip_offset);
};

class Header {
public:
    uint32_t xpress_magic;
    uint32_t orig_size;
    uint32_t encoded_size;
    uint32_t huffman_table_flags;
    uint32_t zero;
    uint32_t session_signature;
    uint32_t block_index;
    uint32_t crc32;

    Header() : xpress_magic(0), orig_size(0), encoded_size(0),
               huffman_table_flags(0), zero(0), session_signature(0),
               block_index(0), crc32(0) {}

    // Calculate and update the CRC32C for the header
    void update_crc() {
        // Calculate CRC over the header except the crc32 field itself
        crc32 = Crc32(reinterpret_cast<const uint8_t*>(this), sizeof(Header) - sizeof(crc32),0);
    }

    // Serialize this header to a binary stream
    void serialize(std::ostream& os) const {
        os.write(reinterpret_cast<const char*>(this), sizeof(Header) - sizeof(crc32));
        uint32_t crc = crc32; // Use the current crc32 value
        os.write(reinterpret_cast<const char*>(&crc), sizeof(crc));
    }

    // Deserialize this header from a binary stream
    void deserialize(std::istream& is) {
        is.read(reinterpret_cast<char*>(this), sizeof(Header));
    }

    // Extract this header from a larger binary buffer
    static Header extract_from_buffer(const std::vector<uint8_t>& buffer, size_t offset) {
        Header header;
        if (offset + sizeof(Header) <= buffer.size()) {
            std::memcpy(&header, buffer.data() + offset, sizeof(Header));
        }
        return header;
    }

    // Insert this header back into a larger buffer
    void insert_into_buffer(std::vector<uint8_t>& buffer, size_t offset) const {
        if (offset + sizeof(Header) <= buffer.size()) {
            std::memcpy(buffer.data() + offset, this, sizeof(Header));
        }
    }
};
// AbfParser.h

#pragma once

#include <cstdint>
#include <vector>
#include <tuple>
#include "BackupLogHeader.h"
#include "BackupFile.h"
#include "VirtualDirectory.h"
#include "BackupFile.h"
#include "BackupLog.h"

#include <crc32c/crc32c.h>

#include <fstream>
#include "../kaitai/kaitaistream.h"
#include "pbix.h"
#include <sstream>

#include "Xpress9Wrapper.h"

class AbfParser {
public:
    static std::vector<uint8_t> get_sqlite(const std::string &path);
    static std::vector<uint8_t> decompress_datamodel(pbix_t::abf_x9_t* datamodel);
private:
    static std::vector<uint8_t> process_chunk(const pbix_t::chunk_t* chunk, uint32_t block_index, uint32_t& block_index_iterator);
    static std::vector<uint8_t> patch_header(const pbix_t::chunk_t* chunk, uint32_t& block_index_iterator);
    static std::vector<uint8_t> read_buffer_bytes(const std::vector<uint8_t>& buffer, long offset, int size);
    static std::vector<uint8_t> trim_buffer(const std::vector<uint8_t>& buffer);
    static std::tuple<int,int> process_backup_log_header(const std::vector<uint8_t> &buffer);
    static std::vector<uint8_t> extract_sqlite_buffer(const std::vector<uint8_t> &buffer, int skip_offset, int virtual_directory_offset, int virtual_directory_size);
};

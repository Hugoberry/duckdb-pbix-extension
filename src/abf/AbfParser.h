// AbfParser.h

#pragma once

#include <cstdint>
#include <vector>
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
    static std::vector<uint8_t> process_data(const std::vector<uint8_t>& decompressed_buffer);
    static std::vector<uint8_t> get_sqlite(const std::string &path);
    static std::vector<uint8_t> decompress_datamodel(pbix_t::abf_x9_t* datamodel);

private:
    static std::vector<uint8_t> read_buffer_bytes(const std::vector<uint8_t>& buffer, long offset, int size);
    static std::vector<uint8_t> trim_buffer(const std::vector<uint8_t>& buffer);
};

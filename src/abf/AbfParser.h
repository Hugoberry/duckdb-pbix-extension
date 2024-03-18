// AbfParser.h

#pragma once

#include <cstdint>
#include <vector>
#include "BackupLogHeader.h"
#include "BackupFile.h"
#include "VirtualDirectory.h"
#include "BackupFile.h"
#include "BackupLog.h"


class AbfParser {
public:
    std::vector<uint8_t> process_data(const std::vector<uint8_t>& decompressed_buffer);

private:
    std::vector<uint8_t> read_buffer_bytes(const std::vector<uint8_t>& buffer, long offset, int size);
    std::vector<uint8_t> trim_buffer(const std::vector<uint8_t>& buffer);
};

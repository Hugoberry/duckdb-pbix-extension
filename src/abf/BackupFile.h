#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include "../../third_party/tinyxml2/tinyxml2.h"

class BackupFile {
public:
    std::optional<std::string> Path;
    int Size;
    uint64_t m_cbOffsetHeader;
    bool Delete;
    int64_t CreatedTimestamp;
    int64_t Access;
    int64_t LastWriteTime;

    static BackupFile from_value(tinyxml2::XMLElement& element, const std::string& encoding);
    static BackupFile from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding);
};

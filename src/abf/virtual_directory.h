#pragma once

#include <vector>
#include <optional>
#include <cstdint>
#include <string>
#include <stdexcept>
#include "tinyxml2.h"
// VirtualDirectory
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
class VirtualDirectory {
public:
    std::optional<std::vector<BackupFile>> backupFiles;

    static VirtualDirectory from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding);
};
// 
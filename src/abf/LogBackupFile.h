#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <tinyxml2.h>

class LogBackupFile {
public:
    std::optional<std::string> Path;
    std::optional<std::string> StoragePath;
    int Size;
    int64_t LastWriteTime;

    static LogBackupFile from_value(tinyxml2::XMLElement& element, const std::string& encoding);
    static LogBackupFile from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding);
};

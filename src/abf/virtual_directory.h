#pragma once

#include <vector>
#include <optional>
#include "backup_file.h"
#include "tinyxml2.h"
// VirtualDirectory
class VirtualDirectory {
public:
    std::optional<std::vector<BackupFile>> backupFiles;

    static VirtualDirectory from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding);
};


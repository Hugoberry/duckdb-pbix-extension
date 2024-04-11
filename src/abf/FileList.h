#pragma once

#include <vector>
#include "LogBackupFile.h"
#include "../../third_party/tinyxml2/tinyxml2.h"

class FileList {
public:
    std::optional<std::vector<LogBackupFile>> BackupFiles;

    static FileList from_xml(tinyxml2::XMLElement& element);
};
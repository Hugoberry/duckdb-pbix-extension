#pragma once

#include <vector>
#include "BackupFile.h"

class FileList {
public:
    std::optional<std::vector<BackupFile>> BackupFiles;

    static FileList from_xml(tinyxml2::XMLElement& element);
};
#pragma once

#include <vector>
#include <string>
#include <optional>
#include <stdexcept>
#include "file_list.h"
#include "tinyxml2.h"

class FileGroup {
public:
    int Class;
    std::optional<std::string> ID;
    std::optional<std::string> Name;
    int ObjectVersion;
    int PersistLocation;
    std::optional<std::string> PersistLocationPath;
    std::optional<std::string> StorageLocationPath;
    std::optional<std::string> ObjectID;  // I'm using string to simplify, but you may want to use a UUID/GUID type
    std::optional<std::vector<FileList>> FileLists;

    static FileGroup from_xml(tinyxml2::XMLElement& element);
};

class FileGroups {
public:
    std::optional<std::vector<FileGroup>> FileGroupList;

    static FileGroups from_xml(tinyxml2::XMLElement& element);
};

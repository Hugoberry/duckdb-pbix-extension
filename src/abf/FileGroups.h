// FileGroups.h
#ifndef FILEGROUPS_H
#define FILEGROUPS_H

#include <vector>
#include <optional>
#include "FileGroup.h"

class FileGroups {
public:
    std::optional<std::vector<FileGroup>> FileGroupList;

    static FileGroups from_xml(tinyxml2::XMLElement& element);
};

#endif // FILEGROUPS_H
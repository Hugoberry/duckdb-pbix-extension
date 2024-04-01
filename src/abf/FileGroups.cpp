// FileGroups.cpp
#include "FileGroups.h"
#include "FileGroup.h"
#include <tinyxml2.h>

using namespace tinyxml2;

FileGroups FileGroups::from_xml(tinyxml2::XMLElement& element) {
    FileGroups fileGroups;

    // Iterate over each "FileGroup" child element
    XMLElement* fileGroupElement = element.FirstChildElement("FileGroup");
    while (fileGroupElement) {
        // Deserialize the FileGroup and add it to the vector
        FileGroup fileGroup = FileGroup::from_xml(*fileGroupElement);
        if (!fileGroups.FileGroupList) {
            fileGroups.FileGroupList = std::vector<FileGroup>{};
        }
        fileGroups.FileGroupList->push_back(fileGroup);

        // Move to the next "FileGroup" sibling element
        fileGroupElement = fileGroupElement->NextSiblingElement("FileGroup");
    }

    return fileGroups;
}

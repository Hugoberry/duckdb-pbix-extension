// FileGroup.cpp
#include "file_group.h"

using namespace tinyxml2;

FileGroup FileGroup::from_xml(tinyxml2::XMLElement& element) {
    FileGroup fileGroup;

    XMLElement* classElement = element.FirstChildElement("Class");
    if (classElement && classElement->GetText()) {
        fileGroup.Class = std::stoi(classElement->GetText());
    }
    else {
        throw std::runtime_error("Invalid XML: Missing or empty 'Class' in 'FileGroup'.");
    }

    XMLElement* idElement = element.FirstChildElement("ID");
    if (idElement && idElement->GetText()) {
        fileGroup.ID = idElement->GetText();
    }

    XMLElement* nameElement = element.FirstChildElement("Name");
    if (nameElement && nameElement->GetText()) {
        fileGroup.Name = nameElement->GetText();
    }

    XMLElement* objectVersionElement = element.FirstChildElement("ObjectVersion");
    if (objectVersionElement && objectVersionElement->GetText()) {
        fileGroup.ObjectVersion = std::stoi(objectVersionElement->GetText());
    }
    else {
        throw std::runtime_error("Invalid XML: Missing or empty 'ObjectVersion' in 'FileGroup'.");
    }

    XMLElement* persistLocationElement = element.FirstChildElement("PersistLocation");
    if (persistLocationElement && persistLocationElement->GetText()) {
        fileGroup.PersistLocation = std::stoi(persistLocationElement->GetText());
    }
    else {
        throw std::runtime_error("Invalid XML: Missing or empty 'PersistLocation' in 'FileGroup'.");
    }

    XMLElement* persistLocationPathElement = element.FirstChildElement("PersistLocationPath");
    if (persistLocationPathElement && persistLocationPathElement->GetText()) {
        fileGroup.PersistLocationPath = persistLocationPathElement->GetText();
    }

    XMLElement* storageLocationPathElement = element.FirstChildElement("StorageLocationPath");
    if (storageLocationPathElement && storageLocationPathElement->GetText()) {
        fileGroup.StorageLocationPath = storageLocationPathElement->GetText();
    }

    XMLElement* objectIDElement = element.FirstChildElement("ObjectID");
    if (objectIDElement && objectIDElement->GetText()) {
        fileGroup.ObjectID = objectIDElement->GetText();
    }

    XMLElement* fileListElement = element.FirstChildElement("FileList");
    while (fileListElement) {
        FileList fileList = FileList::from_xml(*fileListElement);
        if (!fileGroup.FileLists) {
            fileGroup.FileLists = std::vector<FileList>{};
        }
        fileGroup.FileLists->push_back(fileList);
        fileListElement = fileListElement->NextSiblingElement("FileList");
    }

    return fileGroup;
}


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
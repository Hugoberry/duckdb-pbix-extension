// VirtualDirectory.cpp
#include "virtual_directory.h"

using namespace tinyxml2;

VirtualDirectory VirtualDirectory::from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding) {
    XMLDocument doc;
    XMLError error = doc.Parse(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    if (error != XML_SUCCESS) {
        throw std::runtime_error("Error deserializing XML buffer: " + std::to_string(error));
    }

    VirtualDirectory directory;
    XMLElement* root = doc.RootElement();
    if (root) {
        XMLElement* backupFileElement = root->FirstChildElement("BackupFile");
        while (backupFileElement) {
            // Now we're passing the entire BackupFile element to BackupFile::from_value
            BackupFile file = BackupFile::from_value(*backupFileElement, encoding);
            if (!directory.backupFiles) {
                directory.backupFiles = std::vector<BackupFile>{};
            }
            directory.backupFiles->push_back(file);
            backupFileElement = backupFileElement->NextSiblingElement("BackupFile");
        }
    }

    return directory;
}

BackupFile BackupFile::from_value(tinyxml2::XMLElement& element, const std::string& encoding) {
    BackupFile file;
    XMLElement* pathElement = element.FirstChildElement("Path");
    if (pathElement && pathElement->GetText()) {
        file.Path = std::string(pathElement->GetText());
    }
    XMLElement* sizeElement = element.FirstChildElement("Size");
    if (sizeElement && sizeElement->GetText()) {
        file.Size = std::stoi(sizeElement->GetText());
    }
    XMLElement* offsetHeaderElement = element.FirstChildElement("m_cbOffsetHeader");
    if (offsetHeaderElement && offsetHeaderElement->GetText()) {
        file.m_cbOffsetHeader = std::stoull(offsetHeaderElement->GetText());
    }
    XMLElement* deleteElement = element.FirstChildElement("Delete");
    if (deleteElement && deleteElement->GetText()) {
        file.Delete = std::strcmp(deleteElement->GetText(), "true") == 0;
    }
    XMLElement* createdTimestampElement = element.FirstChildElement("CreatedTimestamp");
    if (createdTimestampElement && createdTimestampElement->GetText()) {
        file.CreatedTimestamp = std::stoll(createdTimestampElement->GetText());
    }
    XMLElement* accessElement = element.FirstChildElement("Access");
    if (accessElement && accessElement->GetText()) {
        file.Access = std::stoll(accessElement->GetText());
    }
    XMLElement* lastWriteTimeElement = element.FirstChildElement("LastWriteTime");
    if (lastWriteTimeElement && lastWriteTimeElement->GetText()) {
        file.LastWriteTime = std::stoll(lastWriteTimeElement->GetText());
    }

    return file;
}

BackupFile BackupFile::from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding) {
    XMLDocument doc;
    XMLError error = doc.Parse(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    if (error != XML_SUCCESS) {
        throw std::runtime_error("Error deserializing XML buffer: " + std::to_string(error));
    }

    XMLElement* root = doc.RootElement();
    if (root) {
        return BackupFile::from_value(*root, encoding);
    }

    return BackupFile{};
}

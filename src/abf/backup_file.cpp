// BackupFile.cpp
#include "backup_file.h"

using namespace tinyxml2;

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
        std::cout << "Error deserializing XML buffer: " << error << std::endl;
        return BackupFile{};
    }

    XMLElement* root = doc.RootElement();
    if (root) {
        return BackupFile::from_value(*root, encoding);
    }

    return BackupFile{};
}

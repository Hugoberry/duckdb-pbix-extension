// LogBackupFile.cpp
#include "LogBackupFile.h"
#include "../../third_party/tinyxml2/tinyxml2.h"

using namespace tinyxml2;

LogBackupFile LogBackupFile::from_value(tinyxml2::XMLElement& element, const std::string& encoding) {
    LogBackupFile file;
    XMLElement* pathElement = element.FirstChildElement("Path");
    if (pathElement && pathElement->GetText()) {
        file.Path = std::string(pathElement->GetText());
    }
    XMLElement* sizeElement = element.FirstChildElement("Size");
    if (sizeElement && sizeElement->GetText()) {
        file.Size = std::stoi(sizeElement->GetText());
    }
    XMLElement* storagePathElement = element.FirstChildElement("StoragePath");
    if (storagePathElement && storagePathElement->GetText()) {
        file.StoragePath = std::string(storagePathElement->GetText());
    }
    XMLElement* lastWriteTimeElement = element.FirstChildElement("LastWriteTime");
    if (lastWriteTimeElement && lastWriteTimeElement->GetText()) {
        file.LastWriteTime = std::stoll(lastWriteTimeElement->GetText());
    }

    return file;
}

LogBackupFile LogBackupFile::from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding) {
    XMLDocument doc;
    XMLError error = doc.Parse(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    if (error != XML_SUCCESS) {
        std::cout << "Error deserializing XML buffer: " << error << std::endl;
        return LogBackupFile{};
    }

    XMLElement* root = doc.RootElement();
    if (root) {
        return LogBackupFile::from_value(*root, encoding);
    }

    return LogBackupFile{};
}

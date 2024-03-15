// VirtualDirectory.cpp
#include "VirtualDirectory.h"

using namespace tinyxml2;

VirtualDirectory VirtualDirectory::from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding) {
    XMLDocument doc;
    XMLError error = doc.Parse(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    if (error != XML_SUCCESS) {
        std::cout << "Error deserializing XML buffer: " << error << std::endl;
        return VirtualDirectory{};
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


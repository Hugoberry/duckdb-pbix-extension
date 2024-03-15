#include "FileList.h"

using namespace tinyxml2;

FileList FileList::from_xml(tinyxml2::XMLElement& element) {
    FileList fileList;

    XMLElement* backupFileElement = element.FirstChildElement("BackupFile");
    while (backupFileElement) {
        BackupFile file = BackupFile::from_value(*backupFileElement,"UTF-8");
        if (!fileList.BackupFiles) {
            fileList.BackupFiles = std::vector<BackupFile>{};
        }
        fileList.BackupFiles->push_back(file);
        backupFileElement = backupFileElement->NextSiblingElement("BackupFile");
    }

    return fileList;
}
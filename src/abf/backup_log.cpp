// BackupLog.cpp
#include "backup_log.h"


using namespace tinyxml2;

std::string trimAfterLastTag(const std::string& xmlContent) {
    // Find the last occurrence of the closing tag
    size_t lastTagPos = xmlContent.find_last_of('>');
    if (lastTagPos != std::string::npos) {
        // Trim everything after the last closing tag
        return xmlContent.substr(0, lastTagPos + 1);
    }
    return xmlContent; // Return original if '>' not found
}

BackupLog BackupLog::from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding) {
    std::string buffer_utf8;
    if (encoding == "UTF-16") {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::u16string buffer_utf16(reinterpret_cast<const char16_t*>(buffer.data()), buffer.size() / 2);
        buffer_utf8 = convert.to_bytes(buffer_utf16);
    }
    else {
        buffer_utf8 = std::string(buffer.begin(), buffer.end());
    }

    buffer_utf8 = trimAfterLastTag(buffer_utf8);
    XMLDocument doc;
    doc.Parse(reinterpret_cast<const char*>(buffer_utf8.data()), buffer_utf8.size());

    XMLElement* root = doc.FirstChildElement("BackupLog");
    if (!root) {
        throw std::runtime_error("Invalid XML format: Missing 'BackupLog' root element.");
    }

    BackupLog log;

    XMLElement* element = root->FirstChildElement("BackupRestoreSyncVersion");
    if (element && element->GetText()) {
        log.BackupRestoreSyncVersion = element->GetText();
    }

    element = root->FirstChildElement("ServerRoot");
    if (element && element->GetText()) {
        log.ServerRoot = element->GetText();
    }

    element = root->FirstChildElement("SvrEncryptPwdFlag");
    if (element && element->GetText()) {
        std::istringstream(element->GetText()) >> std::boolalpha >> log.SvrEncryptPwdFlag;
    }

    element = root->FirstChildElement("ServerEnableBinaryXML");
    if (element && element->GetText()) {
		std::istringstream(element->GetText()) >> std::boolalpha >> log.ServerEnableBinaryXML;
	}

    element = root->FirstChildElement("ServerEnableCompression");
    if (element && element->GetText()) {
        std::istringstream(element->GetText()) >> std::boolalpha >> log.ServerEnableCompression;
    }

    element = root->FirstChildElement("CompressionFlag");
    if (element && element->GetText()) {
		std::istringstream(element->GetText()) >> std::boolalpha >> log.CompressionFlag;
	}
    
    element = root->FirstChildElement("EncryptionFlag");
    if (element && element->GetText()) {
        std::istringstream(element->GetText()) >> std::boolalpha >> log.EncryptionFlag;
    }

    element = root->FirstChildElement("ObjectName");
    if (element && element->GetText()) {
		log.ObjectName = element->GetText();
	}

    element = root->FirstChildElement("ObjectId");
    if (element && element->GetText()) {
        log.ObjectId = element->GetText();
    }

    element = root->FirstChildElement("Write");
    if (element && element->GetText()) {
		log.Write = element->GetText();
	}

    element = root->FirstChildElement("OlapInfo");
    if (element && element->GetText()) {
		std::istringstream(element->GetText()) >> std::boolalpha >> log.OlapInfo;
	}

    element = root->FirstChildElement("IsTabular");
    if (element && element->GetText()) {
        std::istringstream(element->GetText()) >> std::boolalpha >> log.IsTabular;
    }
    element = root->FirstChildElement("Collations");
    if (element) {
        log.collations = Collations::from_xml(*element);
    }

    element = root->FirstChildElement("Languages");
    if (element) {
        log.languages = Languages::from_xml(*element);
    }

    element = root->FirstChildElement("FileGroups");
    if (element) {
        log.fileGroups = FileGroups::from_xml(*element);
    }

    return log;
}

Collations Collations::from_xml(tinyxml2::XMLElement& element) {
    Collations collations;

    // Iterate over each "Collation" child element
    XMLElement* collationElement = element.FirstChildElement("Collation");
    while (collationElement) {
        // Get the collation string and add it to the vector
        std::string collation = collationElement->GetText();
        if (!collations.Collation) {
            collations.Collation = std::vector<std::string>{};
        }
        collations.Collation->push_back(collation);

        // Move to the next "Collation" sibling element
        collationElement = collationElement->NextSiblingElement("Collation");
    }

    return collations;
}

Languages Languages::from_xml(tinyxml2::XMLElement& element) {
    Languages languages;

    // Iterate over each "Language" child element
    XMLElement* languageElement = element.FirstChildElement("Language");
    while (languageElement) {
        // Parse the language int and add it to the vector
        int language = std::stoi(languageElement->GetText());
        if (!languages.Language) {
            languages.Language = std::vector<int>{};
        }
        languages.Language->push_back(language);

        // Move to the next "Language" sibling element
        languageElement = languageElement->NextSiblingElement("Language");
    }

    return languages;
}

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

FileList FileList::from_xml(tinyxml2::XMLElement& element) {
    FileList fileList;

    XMLElement* backupFileElement = element.FirstChildElement("BackupFile");
    while (backupFileElement) {
        LogBackupFile file = LogBackupFile::from_value(*backupFileElement,"UTF-8");
        if (!fileList.BackupFiles) {
            fileList.BackupFiles = std::vector<LogBackupFile>{};
        }
        fileList.BackupFiles->push_back(file);
        backupFileElement = backupFileElement->NextSiblingElement("BackupFile");
    }

    return fileList;
}

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
        throw std::runtime_error("Error deserializing XML buffer: " + std::to_string(error));
    }

    XMLElement* root = doc.RootElement();
    if (root) {
        return LogBackupFile::from_value(*root, encoding);
    }

    return LogBackupFile{};
}

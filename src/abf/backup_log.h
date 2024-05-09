#pragma once

#include <string>
#include <optional>
#include <vector>
#include <sstream>
#include <codecvt>
#include <locale>
#include "tinyxml2.h"

// Forward declarations
class BackupFile;
class Collations;
class FileGroup;
class FileGroups;
class FileList;
class Languages;
class LogBackupFile; 

class LogBackupFile {
public:
    std::optional<std::string> Path;
    std::optional<std::string> StoragePath;
    int Size;
    int64_t LastWriteTime;

    static LogBackupFile from_value(tinyxml2::XMLElement& element, const std::string& encoding);
    static LogBackupFile from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding);
};

class Collations {
public:
    std::optional<std::vector<std::string>> Collation;

    static Collations from_xml(tinyxml2::XMLElement& element);
};

class Languages {
public:
    std::optional<std::vector<int>> Language;

    static Languages from_xml(tinyxml2::XMLElement& element);
};

class FileList {
public:
    std::optional<std::vector<LogBackupFile>> BackupFiles;

    static FileList from_xml(tinyxml2::XMLElement& element);
};

class FileGroup {
public:
    int Class;
    std::optional<std::string> ID;
    std::optional<std::string> Name;
    int ObjectVersion;
    int PersistLocation;
    std::optional<std::string> PersistLocationPath;
    std::optional<std::string> StorageLocationPath;
    std::optional<std::string> ObjectID; // GUID
    std::optional<std::vector<FileList>> FileLists;

    static FileGroup from_xml(tinyxml2::XMLElement& element);
};


class FileGroups {
public:
    std::optional<std::vector<FileGroup>> FileGroupList;

    static FileGroups from_xml(tinyxml2::XMLElement& element);
};
class BackupLog {
public:
    std::optional<std::string> BackupRestoreSyncVersion;
    std::optional<std::string> ServerRoot;
    bool SvrEncryptPwdFlag;
    bool ServerEnableBinaryXML;
    bool ServerEnableCompression;
    bool CompressionFlag;
    bool EncryptionFlag;
    std::optional<std::string> ObjectName;
    std::optional<std::string> ObjectId;
    std::optional<std::string> Write;
    bool OlapInfo;
    bool IsTabular;
    std::optional<Collations> collations;
    std::optional<Languages> languages;
    std::optional<FileGroups> fileGroups;

    static BackupLog from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding);
};

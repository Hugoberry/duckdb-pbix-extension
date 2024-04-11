// BackupLog.h
#ifndef BACKUPLOG_H
#define BACKUPLOG_H

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <string>
#include <optional>
#include <sstream>
#include "Collations.h"
#include "Languages.h"
#include "FileGroups.h"
#include <codecvt>
#include <locale>
#include "../../third_party/tinyxml2/tinyxml2.h"

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

#endif // BACKUPLOG_H

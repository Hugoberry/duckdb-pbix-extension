#pragma once

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <cstdint>
#include <string>
#include "tinyxml2.h"
#include <vector>
#include <optional>
#include <iostream>
#include <codecvt>
#include <locale>

class BackupLogHeader {
public:
    int BackupRestoreSyncVersion;
    bool Fault;
    uint32_t faultcode;
    bool ErrorCode;
    bool EncryptionFlag;
    int EncryptionKey;
    bool ApplyCompression;
    uint64_t m_cbOffsetHeader;
    uint64_t DataSize;
    uint32_t Files;
    std::optional<std::string> ObjectID; // Convert the Guid to a string for simplicity
    uint64_t m_cbOffsetData;

    static BackupLogHeader from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding);
};
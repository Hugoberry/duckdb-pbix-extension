#include "backup_log_header.h"

using namespace tinyxml2;

BackupLogHeader BackupLogHeader::from_xml(const std::vector<uint8_t>& buffer, const std::string& encoding) {
    std::string buffer_utf8;
    if (encoding == "UTF-16") {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::u16string buffer_utf16(reinterpret_cast<const char16_t*>(buffer.data()), buffer.size() / 2);
        buffer_utf8 = convert.to_bytes(buffer_utf16);
    }
    else {
        buffer_utf8 = std::string(buffer.begin(), buffer.end());
    }

    XMLDocument doc;
    XMLError error = doc.Parse(buffer_utf8.data(), buffer_utf8.size());
    if (error != XML_SUCCESS) {
        std::cout << "Error deserializing XML buffer: " << error << std::endl;
        return BackupLogHeader{};
    }

    BackupLogHeader header;
    XMLElement* root = doc.RootElement();
    if (root) {
        XMLElement* element = nullptr;

        element = root->FirstChildElement("BackupRestoreSyncVersion");
        header.BackupRestoreSyncVersion = element ? std::stoi(element->GetText()) : 0;

        element = root->FirstChildElement("Fault");
        header.Fault = element && std::string(element->GetText()) == "true";

        element = root->FirstChildElement("faultcode");
        header.faultcode = element ? std::stoul(element->GetText()) : 0;

        element = root->FirstChildElement("ErrorCode");
        header.ErrorCode = element && std::string(element->GetText()) == "true";

        element = root->FirstChildElement("EncryptionFlag");
        header.EncryptionFlag = element && std::string(element->GetText()) == "true";

        element = root->FirstChildElement("EncryptionKey");
        header.EncryptionKey = element ? std::stoi(element->GetText()) : 0;

        element = root->FirstChildElement("ApplyCompression");
        header.ApplyCompression = element && std::string(element->GetText()) == "true";

        element = root->FirstChildElement("m_cbOffsetHeader");
        header.m_cbOffsetHeader = element ? std::stoull(element->GetText()) : 0;

        element = root->FirstChildElement("DataSize");
        header.DataSize = element ? std::stoull(element->GetText()) : 0;

        element = root->FirstChildElement("Files");
        header.Files = element ? std::stoul(element->GetText()) : 0;

        element = root->FirstChildElement("ObjectID");
        if (element && element->GetText()) {
            header.ObjectID = std::string(element->GetText());
        }

        element = root->FirstChildElement("m_cbOffsetData");
        header.m_cbOffsetData = element ? std::stoull(element->GetText()) : 0;
    }

    return header;
}
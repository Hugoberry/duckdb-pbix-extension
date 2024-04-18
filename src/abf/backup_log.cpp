// BackupLog.cpp
#include "backup_log.h"


using namespace tinyxml2;

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

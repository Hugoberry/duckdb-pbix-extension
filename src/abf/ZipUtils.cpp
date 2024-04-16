#include "ZipUtils.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstring>

class ZipUtils {
public:
    static bool findEndOfCentralDirectory(std::istream& stream, EndOfCentralDirectoryRecord& eocd) {
        const uint32_t signatureEOCD = 0x06054b50;
        std::vector<char> buffer(4096);
        stream.seekg(0, std::ios::end);
        std::streampos fileSize = stream.tellg();
        int64_t searchOffset = std::min(static_cast<int64_t>(fileSize), static_cast<int64_t>(buffer.size()));

        stream.seekg(-searchOffset, std::ios::end);
        stream.read(buffer.data(), searchOffset);
        auto foundPos = std::search(buffer.rbegin(), buffer.rend(), reinterpret_cast<const char*>(&signatureEOCD), reinterpret_cast<const char*>(&signatureEOCD) + sizeof(signatureEOCD));

        if (foundPos != buffer.rend()) {
            size_t offset = std::distance(buffer.begin(), foundPos.base()) - sizeof(signatureEOCD);
            stream.seekg(-searchOffset + offset, std::ios::end);
            stream.read(reinterpret_cast<char*>(&eocd), sizeof(eocd));
            return true;
        }
        return false;
    }

    static std::pair<uint32_t, uint32_t> findDataModel(std::istream& zipStream) {
        EndOfCentralDirectoryRecord eocd;
        if (!findEndOfCentralDirectory(zipStream, eocd)) {
            throw std::runtime_error("End of central directory not found.");
        }

        zipStream.seekg(eocd.centralDirectoryOffset, std::ios::beg);
        CentralDirectoryFileHeader cdHeader;

        for (int i = 0; i < eocd.numEntries; ++i) {
            zipStream.read(reinterpret_cast<char*>(&cdHeader), sizeof(cdHeader));
            if (cdHeader.signature != 0x02014b50) {
                throw std::runtime_error("Invalid central directory file header signature.");
            }

            std::vector<char> filename(cdHeader.fileNameLength);
            zipStream.read(filename.data(), cdHeader.fileNameLength);
            zipStream.ignore(cdHeader.extraFieldLength + cdHeader.fileCommentLength);

            if (std::string(filename.begin(), filename.end()) == "DataModel") {
                return {cdHeader.localHeaderOffset, cdHeader.compressedSize};
            }
        }

        throw std::runtime_error("DataModel not found in the zip file.");
    }
};

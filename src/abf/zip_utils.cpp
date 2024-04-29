#include "zip_utils.h"

bool ZipUtils::findEndOfCentralDirectory(duckdb::FileHandle &file_handle, ZipUtils::EndOfCentralDirectoryRecord &eocd)
{
    const uint32_t signatureEOCD = 0x504b0506;
    // const uint32_t signatureEOCD = 0x06054b50;
    std::vector<char> buffer(4096);
    idx_t fileSize = file_handle.GetFileSize();
    int64_t searchOffset = std::min(static_cast<int64_t>(fileSize), static_cast<int64_t>(buffer.size()));

    file_handle.Seek(fileSize - searchOffset);
    file_handle.Read(buffer.data(), searchOffset);

    auto foundPos = std::search(buffer.rbegin(), buffer.rend(), reinterpret_cast<const char *>(&signatureEOCD), reinterpret_cast<const char *>(&signatureEOCD) + sizeof(signatureEOCD));

    if (foundPos != buffer.rend())
    {
        idx_t offset = std::distance(buffer.begin(), foundPos.base()) - sizeof(signatureEOCD);
        file_handle.Seek(fileSize - searchOffset + offset);
        file_handle.Read(reinterpret_cast<char *>(&eocd), sizeof(eocd));
        return true;
    }
    return false;
}

std::pair<uint32_t, uint32_t> ZipUtils::findDataModel(duckdb::FileHandle &file_handle)
{
    ZipUtils::EndOfCentralDirectoryRecord eocd;
    if (!findEndOfCentralDirectory(file_handle, eocd))
    {
        throw std::runtime_error("End of central directory not found.");
    }

    file_handle.Seek(eocd.centralDirectoryOffset);
    idx_t position = eocd.centralDirectoryOffset;

    for (int i = 0; i < eocd.numEntries; ++i)
    {
        std::vector<char> headerBuffer(sizeof(ZipUtils::CentralDirectoryFileHeader));
        file_handle.Read(headerBuffer.data(), sizeof(ZipUtils::CentralDirectoryFileHeader));

        ZipUtils::CentralDirectoryFileHeader cdHeader;
        memcpy(&cdHeader, headerBuffer.data(), sizeof(cdHeader));

        std::vector<char> filename(cdHeader.fileNameLength);
        file_handle.Read(filename.data(), cdHeader.fileNameLength);

        position += sizeof(cdHeader) + cdHeader.fileNameLength;

        if (std::string(filename.begin(), filename.end()) == "DataModel")
        {
            uint32_t localHeaderOffset = cdHeader.localHeaderOffset; // Make a copy because packed struct
            uint32_t compressedSize = cdHeader.compressedSize;       // Make a copy because packed struct
            return {localHeaderOffset, compressedSize};
        }

        // Move to the next entry, skipping over the extra fields and comments
        position += cdHeader.extraFieldLength + cdHeader.fileCommentLength;
        file_handle.Seek(position); // Seek to the next header position
    }

    throw std::runtime_error("DataModel not found in the zip file.");
}

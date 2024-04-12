#ifndef ZIP_UTILS_H
#define ZIP_UTILS_H

#include <iostream>
#include <fstream>
#include <vector>

// Structure to store the end of central directory record.
struct EndOfCentralDirectoryRecord {
    uint32_t signature;
    uint16_t diskNumber;
    uint16_t centralDirectoryDiskNumber;
    uint16_t numEntriesThisDisk;
    uint16_t numEntries;
    uint32_t centralDirectorySize;
    uint32_t centralDirectoryOffset;
    uint16_t commentLength;
};

// Structure to store the central directory file header.
struct CentralDirectoryFileHeader {
    uint32_t signature;
    uint16_t versionMadeBy;
    uint16_t versionNeededToExtract;
    uint16_t generalPurposeBitFlag;
    uint16_t compressionMethod;
    uint16_t lastModFileTime;
    uint16_t lastModFileDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t extraFieldLength;
    uint16_t fileCommentLength;
    uint16_t diskNumberStart;
    uint16_t internalFileAttributes;
    uint32_t externalFileAttributes;
    uint32_t localHeaderOffset;
};

// Class containing static utility methods for working with ZIP files.
class ZipUtils {
public:
    static bool findEndOfCentralDirectory(std::istream& stream, EndOfCentralDirectoryRecord& eocd);
    static std::pair<uint32_t, uint32_t> findDataModel(std::istream& zipStream);
};

#endif // ZIP_UTILS_H

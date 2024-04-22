#include "abf_parser.h"
// #include "duckdb/common/file_system.hpp"

using namespace tinyxml2;
using namespace duckdb;

std::vector<uint8_t> AbfParser::read_buffer_bytes(const std::vector<uint8_t> &buffer, uint64_t offset, int size)
{
    if (offset < 0 || size < 0 || static_cast<size_t>(offset) + size > buffer.size())
    {
        throw std::out_of_range("Offset and size must be within the bounds of the buffer.");
    }
    std::vector<uint8_t> sub_buffer(size);
    std::copy(buffer.begin() + offset, buffer.begin() + offset + size, sub_buffer.begin());
    return sub_buffer;
}

std::vector<uint8_t> AbfParser::trim_buffer(const std::vector<uint8_t> &buffer)
{
    int last_non_zero_byte = buffer.size() - 1;
    while (last_non_zero_byte >= 1 && buffer[last_non_zero_byte] == 0x00 && buffer[last_non_zero_byte - 1] == 0x00)
    {
        last_non_zero_byte -= 2;
    }
    last_non_zero_byte++;
    std::vector<uint8_t> trimmed_buffer(buffer.begin(), buffer.begin() + last_non_zero_byte + 1);
    return trimmed_buffer;
}

std::tuple<uint64_t, int> AbfParser::process_backup_log_header(const std::vector<uint8_t> &buffer)
{

    std::vector<uint8_t> backup_log_header_buffer = read_buffer_bytes(buffer, ABF_BACKUP_LOG_HEADER_OFFSET, ABF_BACKUP_LOG_HEADER_SIZE);
    backup_log_header_buffer = trim_buffer(backup_log_header_buffer);
    BackupLogHeader backup_log_header = BackupLogHeader::from_xml(backup_log_header_buffer, "UTF-16");
    return {backup_log_header.m_cbOffsetHeader, backup_log_header.DataSize};
}

std::tuple<std::vector<uint8_t>, std::vector<VertipaqFile>>  AbfParser::extract_sqlite_buffer(const std::vector<uint8_t> &buffer, uint64_t skip_offset, uint64_t virtual_directory_offset, int virtual_directory_size)
{
    std::vector<uint8_t> virtual_directory_buffer = read_buffer_bytes(buffer, virtual_directory_offset - skip_offset, virtual_directory_size);
    VirtualDirectory virtual_directory = VirtualDirectory::from_xml(virtual_directory_buffer, "UTF-8");

    // Validate the presence of backup files before proceeding.
    if (virtual_directory.backupFiles->empty())
    {
        throw std::runtime_error("No backup files found in virtual directory.");
    }

    const BackupFile &log = virtual_directory.backupFiles->back();
    uint64_t backup_file_offset = log.m_cbOffsetHeader;
    int backup_file_size = log.Size;

    std::vector<uint8_t> backup_log_buffer = read_buffer_bytes(buffer, backup_file_offset + 2 - skip_offset, backup_file_size - 2);
    BackupLog backup_log = BackupLog::from_xml(backup_log_buffer, "UTF-16");

    // Ensure there are enough backup files to safely access the second to last one.
    if (virtual_directory.backupFiles->size() < 2)
    {
        throw std::runtime_error("Insufficient backup files for SQLite extraction.");
    }

    auto sqlite = (*virtual_directory.backupFiles)[virtual_directory.backupFiles->size() - 2];
    uint64_t sqlite_offset = sqlite.m_cbOffsetHeader;
    int sqlite_size = sqlite.Size;

    return {read_buffer_bytes(buffer, sqlite_offset - skip_offset, sqlite_size), match_logs_and_get_vertipaq_meta(backup_log, virtual_directory)};
}


std::vector<VertipaqFile> AbfParser::match_logs_and_get_vertipaq_meta(const BackupLog& backupLog, const VirtualDirectory& virtualDirectory)
{

    std::vector<VertipaqFile> matchedData;
    // Check if fileGroups and backupFiles are populated
    if (!backupLog.fileGroups.has_value() || !virtualDirectory.backupFiles.has_value()) {
        std::cerr << "Missing file groups or backup files." << std::endl;
        return matchedData;
    }

    const auto& fileGroups = backupLog.fileGroups->FileGroupList.value();
    const auto& backupFiles = virtualDirectory.backupFiles.value();
    
    // Create a map from storage paths to backup files
    std::map<std::string, BackupFile> virtualDirectoryFilesByPath;
    for (const auto& file : backupFiles) {
        virtualDirectoryFilesByPath[file.Path.value()] = file;
    }

    // Loop through each file group and each file within the group
    for (const auto& fileGroup : fileGroups) {
        if (!fileGroup.FileLists.has_value()) continue;
        for (const auto& fileList : fileGroup.FileLists.value()) {
            for (const auto& backupFile : fileList.BackupFiles.value()) {
                if (!backupFile.StoragePath.has_value()) continue;
                auto it = virtualDirectoryFilesByPath.find(backupFile.StoragePath.value());
                if (it != virtualDirectoryFilesByPath.end()) {
                    const BackupFile& matchedFile = it->second;
                    std::string pathWithoutPersistRoot = backupFile.Path.value_or("");
                    VertipaqFile data;
                    data.Path = pathWithoutPersistRoot;
                    data.FileName = pathWithoutPersistRoot.substr(pathWithoutPersistRoot.find_last_of('\\') + 1);
                    data.StoragePath = backupFile.StoragePath.value();
                    data.Size = matchedFile.Size;
                    data.m_cbOffsetHeader = matchedFile.m_cbOffsetHeader;

                    matchedData.push_back(data);
                }
            }
        }
    }

    return matchedData;

}

void AbfParser::patch_header_of_compressed_buffer(std::vector<uint8_t> &compressed_buffer, uint32_t &block_index_iterator)
{
    block_index_iterator++;

    Header header = Header::extract_from_buffer(compressed_buffer, 0);
    header.block_index = block_index_iterator;
    header.update_crc();
    header.insert_into_buffer(compressed_buffer, 0);
}

void AbfParser::read_compressed_datamodel_header(std::ifstream &entryStream, uint64_t &datamodel_ofs)
{
    // Read compressed DataModel header to adjust offset
    entryStream.seekg(datamodel_ofs + ZIP_LOCAL_FILE_HEADER_FIXED);
    uint16_t filename_len = 0;
    uint16_t extra_len = 0;
    entryStream.read(reinterpret_cast<char *>(&filename_len), sizeof(filename_len));
    entryStream.read(reinterpret_cast<char *>(&extra_len), sizeof(extra_len));

    datamodel_ofs += ZIP_LOCAL_FILE_HEADER + filename_len + extra_len;
}

std::vector<uint8_t> AbfParser::decompress_initial_block(duckdb::FileHandle &file_handle_p, uint64_t &bytes_read, XPress9Wrapper &xpress9_wrapper)
{
    // Seek to the start of the DataModel compressed data
    std::vector<uint8_t> signature(ABF_XPRESS9_SIGNATURE);
    file_handle_p.Read(reinterpret_cast<char *>(signature.data()), ABF_XPRESS9_SIGNATURE);

    bytes_read += ABF_XPRESS9_SIGNATURE;

    uint32_t uncompressed_size;
    uint32_t compressed_size;
    // Read the compressed and uncompressed sizes before the offset
    file_handle_p.Read(reinterpret_cast<char *>(&uncompressed_size), sizeof(uint32_t));
    file_handle_p.Read(reinterpret_cast<char *>(&compressed_size), sizeof(uint32_t));
    bytes_read += sizeof(uint32_t) + sizeof(uint32_t);

    // Allocate buffers for compressed and decompressed data
    std::vector<uint8_t> decompressed_buffer(uncompressed_size);
    std::vector<uint8_t> compressed_buffer(compressed_size);

    file_handle_p.Read(reinterpret_cast<char *>(compressed_buffer.data()), compressed_size);
    bytes_read += compressed_size;

    // Decompress the entire data
    uint32_t decompressed_size = xpress9_wrapper.Decompress(compressed_buffer.data(), compressed_size, decompressed_buffer.data(), decompressed_buffer.size());
    // Verify that the total decompressed size matches the expected size
    if (decompressed_size != uncompressed_size)
    {
        throw std::runtime_error("Mismatch in decompressed block size in first block.");
    }
    // file_handle = file_handle_p.release();
    return decompressed_buffer;
}

std::vector<uint8_t> AbfParser::iterate_and_decompress_blocks(duckdb::FileHandle &file_handle_p, uint64_t &bytes_read, uint64_t datamodel_ofs, uint64_t datamodel_size, XPress9Wrapper &xpress9_wrapper, uint64_t virtual_directory_offset, int virtual_directory_size, const int trailing_blocks, uint64_t &skip_offset)
{
    // Calculate the total number of blocks

    auto total_blocks = (virtual_directory_size + virtual_directory_offset) / BLOCK_SIZE;

    std::vector<uint8_t> all_decompressed_data;
    uint32_t block_index = 0;
    uint32_t block_index_iterator = 0;

    // Iterate through each block in the DataModel
    while (bytes_read < datamodel_size)
    {
        block_index++;
        // Read the compressed and uncompressed sizes
        uint32_t uncompressed_size = 0;
        uint32_t compressed_size = 0;
        file_handle_p.Read(reinterpret_cast<char *>(&uncompressed_size), sizeof(uncompressed_size));
        file_handle_p.Read(reinterpret_cast<char *>(&compressed_size), sizeof(compressed_size));
        bytes_read += sizeof(uncompressed_size) + sizeof(compressed_size);

        // Skip blocks if not within the last `trailing_blocks` (based on your logic)
        if (total_blocks > trailing_blocks && block_index < (total_blocks - trailing_blocks))
        {
            skip_offset += uncompressed_size;
            bytes_read += compressed_size;
            file_handle_p.Seek(datamodel_ofs + bytes_read); // Skip this block
            continue;
        }

        // Allocate buffers for the compressed and decompressed data
        std::vector<uint8_t> compressed_buffer(compressed_size);
        std::vector<uint8_t> decompressed_buffer(uncompressed_size);

        // Read the compressed block
        file_handle_p.Read(reinterpret_cast<char *>(compressed_buffer.data()), compressed_size);
        bytes_read += compressed_size;

        // call to a new function process header_buffer which we'll use to modify compressed_buffer
        patch_header_of_compressed_buffer(compressed_buffer, block_index_iterator);

        // Decompress the block
        uint32_t decompressed_size = xpress9_wrapper.Decompress(compressed_buffer.data(), compressed_size, decompressed_buffer.data(), decompressed_buffer.size());

        // Verify decompression success
        if (decompressed_size != uncompressed_size)
        {
            throw std::runtime_error("Decompression failed or resulted in unexpected size.");
        }

        // Add decompressed data to the overall buffer
        all_decompressed_data.insert(all_decompressed_data.end(), decompressed_buffer.begin(), decompressed_buffer.end());
    }

    // file_handle = file_handle_p.release();

    return all_decompressed_data;
}

std::tuple<std::vector<uint8_t>, std::vector<VertipaqFile>>  AbfParser::get_sqlite(duckdb::ClientContext &context, const std::string &path, const int trailing_blocks = 15)
{
    auto &fs = duckdb::FileSystem::GetFileSystem(context);
    // Open the file using FileSystem
    auto file_handle = fs.OpenFile(path, FILE_READ);
    if (!file_handle)
    {
        throw std::runtime_error("Could not open zip file");
    }

    ZipUtils::EndOfCentralDirectoryRecord eocd;

    // Find the end of central directory record
    if (!ZipUtils::findEndOfCentralDirectory(*file_handle, eocd))
    {
        throw std::runtime_error("End of central directory not found.");
    }

    // Now, try to find a specific file in the ZIP, for example "DataModel"
    auto [datamodel_ofs, datamodel_size] = ZipUtils::findDataModel(*file_handle);


    uint64_t bytes_read = 0;
    uint16_t zip_pointer = 0;

    // Read compressed DataModel header to adjust offset
    file_handle->Seek(datamodel_ofs + ZIP_LOCAL_FILE_HEADER_FIXED);
    uint16_t filename_len = 0;
    uint16_t extra_len = 0;
    file_handle->Read(reinterpret_cast<char *>(&filename_len), sizeof(filename_len));
    file_handle->Read(reinterpret_cast<char *>(&extra_len), sizeof(extra_len));
    datamodel_ofs += ZIP_LOCAL_FILE_HEADER + filename_len + extra_len;

    file_handle->Seek(datamodel_ofs);

    XPress9Wrapper xpress9_wrapper;
    if (!xpress9_wrapper.Initialize())
    {
        throw std::runtime_error("Failed to initialize XPress9Wrapper");
    }

    // Decompress initial block to get the virtual directory info
    auto initial_decompressed_buffer = decompress_initial_block(*file_handle, bytes_read, xpress9_wrapper);

    // Process backup log header to get virtual directory offset and size
    auto [virtual_directory_offset, virtual_directory_size] = process_backup_log_header(initial_decompressed_buffer);

    uint64_t skip_offset = 0; // optimization for skipping blocks
    // Iterate through the remaining blocks and decompress them
    auto all_decompressed_buffer = iterate_and_decompress_blocks(*file_handle, bytes_read, datamodel_ofs, datamodel_size, xpress9_wrapper, virtual_directory_offset, virtual_directory_size, trailing_blocks, skip_offset);

    // Prefix all_decompressed_buffer with initial_decompressed_buffer in case we have only one block
    all_decompressed_buffer.insert(all_decompressed_buffer.begin(), initial_decompressed_buffer.begin(), initial_decompressed_buffer.end());

    if (skip_offset + all_decompressed_buffer.size() < virtual_directory_offset + virtual_directory_size)
    {
        throw std::runtime_error("Could not parse the entire DataModel.");
    }
    // Finally, extract the SQLite buffer from the decompressed data
    return extract_sqlite_buffer(all_decompressed_buffer, skip_offset, virtual_directory_offset, virtual_directory_size);
}
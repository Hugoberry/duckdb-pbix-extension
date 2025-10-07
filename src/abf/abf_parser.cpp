#include "abf_parser.h"

using namespace tinyxml2;
using namespace duckdb;

// Signature constants as UTF-16LE byte patterns
const std::vector<uint8_t> AbfParser::SINGLE_THREAD_SIGNATURE_BYTES = {
    0x54, 0x00, 0x68, 0x00, 0x69, 0x00, 0x73, 0x00, 0x20, 0x00, 0x62, 0x00, 0x61, 0x00, 0x63, 0x00,
    0x6B, 0x00, 0x75, 0x00, 0x70, 0x00, 0x20, 0x00, 0x77, 0x00, 0x61, 0x00, 0x73, 0x00, 0x20, 0x00,
    0x63, 0x00, 0x72, 0x00, 0x65, 0x00, 0x61, 0x00, 0x74, 0x00, 0x65, 0x00, 0x64, 0x00, 0x20, 0x00,
    0x75, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x20, 0x00, 0x58, 0x00, 0x50, 0x00,
    0x72, 0x00, 0x65, 0x00, 0x73, 0x00, 0x73, 0x00, 0x39, 0x00, 0x20, 0x00, 0x63, 0x00, 0x6F, 0x00,
    0x6D, 0x00, 0x70, 0x00, 0x72, 0x00, 0x65, 0x00, 0x73, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6F, 0x00,
    0x6E, 0x00, 0x2E, 0x00
}; // "This backup was created using XPress9 compression."

const std::vector<uint8_t> AbfParser::MULTI_THREAD_SIGNATURE_BYTES = {
    0x54, 0x00, 0x68, 0x00, 0x69, 0x00, 0x73, 0x00, 0x20, 0x00, 0x62, 0x00, 0x61, 0x00, 0x63, 0x00,
    0x6B, 0x00, 0x75, 0x00, 0x70, 0x00, 0x20, 0x00, 0x77, 0x00, 0x61, 0x00, 0x73, 0x00, 0x20, 0x00,
    0x63, 0x00, 0x72, 0x00, 0x65, 0x00, 0x61, 0x00, 0x74, 0x00, 0x65, 0x00, 0x64, 0x00, 0x20, 0x00,
    0x75, 0x00, 0x73, 0x00, 0x69, 0x00, 0x6E, 0x00, 0x67, 0x00, 0x20, 0x00, 0x6D, 0x00, 0x75, 0x00,
    0x6C, 0x00, 0x74, 0x00, 0x69, 0x00, 0x74, 0x00, 0x68, 0x00, 0x72, 0x00, 0x65, 0x00, 0x61, 0x00,
    0x64, 0x00, 0x65, 0x00, 0x64, 0x00, 0x20, 0x00, 0x58, 0x00, 0x50, 0x00, 0x72, 0x00, 0x73, 0x00,
    0x39, 0x00, 0x2E, 0x00
}; // "This backup was created using multithreaded XPrs9."

const std::vector<uint8_t> AbfParser::STREAM_STORAGE_SIGNATURE_BYTES = {
    0xFF, 0xFE, // UTF-16LE BOM
    0x53, 0x00, 0x54, 0x00, 0x52, 0x00, 0x45, 0x00, 0x41, 0x00, 0x4D, 0x00, 0x5F, 0x00, 0x53, 0x00,
    0x54, 0x00, 0x4F, 0x00, 0x52, 0x00, 0x41, 0x00, 0x47, 0x00, 0x45, 0x00, 0x5F, 0x00, 0x53, 0x00,
    0x49, 0x00, 0x47, 0x00, 0x4E, 0x00, 0x41, 0x00, 0x54, 0x00, 0x55, 0x00, 0x52, 0x00, 0x45, 0x00,
    0x5F, 0x00, 0x29, 0x00, 0x21, 0x00, 0x40, 0x00, 0x23, 0x00, 0x24, 0x00, 0x25, 0x00, 0x5E, 0x00,
    0x26, 0x00, 0x2A, 0x00, 0x28, 0x00
}; // "STREAM_STORAGE_SIGNATURE_)!@#$%^&*("

enum class DataModelType {
    SINGLE_THREADED,
    MULTI_THREADED,
    UNCOMPRESSED,
    UNKNOWN
};

static DataModelType DetectDataModelType(duckdb::FileHandle &file_handle, uint64_t datamodel_ofs) {
    // Read signature bytes
    std::vector<uint8_t> signature(ABF_XPRESS9_SIGNATURE);
    file_handle.Seek(datamodel_ofs);
    file_handle.Read(reinterpret_cast<char *>(signature.data()), ABF_XPRESS9_SIGNATURE);
    
    // Check for compressed file types (single-threaded or multi-threaded)
    if (std::equal(AbfParser::SINGLE_THREAD_SIGNATURE_BYTES.begin(), AbfParser::SINGLE_THREAD_SIGNATURE_BYTES.end(), signature.begin())) {
        return DataModelType::SINGLE_THREADED;
    }

    if (std::equal(AbfParser::MULTI_THREAD_SIGNATURE_BYTES.begin(), AbfParser::MULTI_THREAD_SIGNATURE_BYTES.end(), signature.begin())) {
        return DataModelType::MULTI_THREADED;
    }
    
    // Check for uncompressed ABF backup (look for STREAM_STORAGE_SIGNATURE in first 72 bytes)
    file_handle.Seek(datamodel_ofs);
    std::vector<uint8_t> header(ABF_BACKUP_LOG_HEADER_OFFSET);
    file_handle.Read(reinterpret_cast<char *>(header.data()), ABF_BACKUP_LOG_HEADER_OFFSET);
    
    auto it = std::search(header.begin(), header.end(), 
                         AbfParser::STREAM_STORAGE_SIGNATURE_BYTES.begin(), 
                         AbfParser::STREAM_STORAGE_SIGNATURE_BYTES.end());
    if (it != header.end()) {
        return DataModelType::UNCOMPRESSED;
    }
    
    return DataModelType::UNKNOWN;
}

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

BackupLogHeader AbfParser::process_backup_log_header(const std::vector<uint8_t> &buffer)
{
    std::vector<uint8_t> backup_log_header_buffer = read_buffer_bytes(buffer, ABF_BACKUP_LOG_HEADER_OFFSET, ABF_BACKUP_LOG_HEADER_SIZE);
    backup_log_header_buffer = trim_buffer(backup_log_header_buffer);
    return BackupLogHeader::from_xml(backup_log_header_buffer, "UTF-16");
}

DataModel AbfParser::extract_sqlite_buffer(const std::vector<uint8_t> &buffer, uint64_t skip_offset, BackupLogHeader virtual_dir)
{
    std::vector<uint8_t> virtual_directory_buffer = read_buffer_bytes(buffer, virtual_dir.m_cbOffsetHeader - skip_offset, virtual_dir.DataSize);
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

    DataModel data_model = {
        read_buffer_bytes(buffer, sqlite_offset - skip_offset, sqlite_size),
        match_logs_and_get_vertipaq_meta(backup_log, virtual_directory),
        virtual_dir.ErrorCode
    };

    return data_model;
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

// Multi-threaded version that patches session signature
void AbfParser::patch_header_of_compressed_buffer(std::vector<uint8_t> &compressed_buffer, uint32_t &block_index_iterator, uint32_t session_signature)
{
    block_index_iterator++;

    Header header = Header::extract_from_buffer(compressed_buffer, 0);
    header.block_index = block_index_iterator;
    header.session_signature = session_signature;
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
    return decompressed_buffer;
}

std::vector<uint8_t> AbfParser::iterate_and_decompress_blocks(duckdb::FileHandle &file_handle_p, uint64_t &bytes_read, uint64_t datamodel_ofs, uint64_t datamodel_size, XPress9Wrapper &xpress9_wrapper, BackupLogHeader virtual_directory, const int trailing_blocks, uint64_t &skip_offset)
{
    // Calculate the total number of blocks
    auto total_blocks = (virtual_directory.DataSize + virtual_directory.m_cbOffsetHeader) / BLOCK_SIZE;

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

        // Skip blocks if not within the last `trailing_blocks`
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

    return all_decompressed_data;
}

std::vector<uint8_t> AbfParser::decompress_initial_block_multithread(duckdb::FileHandle &file_handle_p, uint64_t &bytes_read, XPress9Wrapper &xpress9_wrapper, MultiThreadMetadata &metadata, uint32_t &first_session_signature)
{
    // Seek to the start of the DataModel compressed data (skip signature)
    std::vector<uint8_t> signature(ABF_XPRESS9_SIGNATURE);
    file_handle_p.Read(reinterpret_cast<char *>(signature.data()), ABF_XPRESS9_SIGNATURE);
    bytes_read += ABF_XPRESS9_SIGNATURE;

    // Read multi-threaded metadata
    file_handle_p.Read(reinterpret_cast<char *>(&metadata.tail_block_count), sizeof(uint64_t));
    file_handle_p.Read(reinterpret_cast<char *>(&metadata.head_block_count), sizeof(uint64_t));
    file_handle_p.Read(reinterpret_cast<char *>(&metadata.head_thread_count), sizeof(uint64_t));
    file_handle_p.Read(reinterpret_cast<char *>(&metadata.tail_thread_count), sizeof(uint64_t));
    file_handle_p.Read(reinterpret_cast<char *>(&metadata.chunk_uncompressed_size), sizeof(uint64_t));
    bytes_read += ABF_MULTITHREAD_METADATA_SIZE;

    // Decompress the first chunk to get backup log header
    uint32_t uncompressed_size;
    uint32_t compressed_size;
    file_handle_p.Read(reinterpret_cast<char *>(&uncompressed_size), sizeof(uint32_t));
    file_handle_p.Read(reinterpret_cast<char *>(&compressed_size), sizeof(uint32_t));
    bytes_read += sizeof(uint32_t) + sizeof(uint32_t);

    // Allocate buffers for compressed and decompressed data
    std::vector<uint8_t> decompressed_buffer(uncompressed_size);
    std::vector<uint8_t> compressed_buffer(compressed_size);

    file_handle_p.Read(reinterpret_cast<char *>(compressed_buffer.data()), compressed_size);
    bytes_read += compressed_size;

    // Extract session signature from first chunk before decompressing
    Header first_header = Header::extract_from_buffer(compressed_buffer, 0);
    first_session_signature = first_header.session_signature;

    // Decompress the first chunk
    uint32_t decompressed_size = xpress9_wrapper.Decompress(compressed_buffer.data(), compressed_size, decompressed_buffer.data(), decompressed_buffer.size());
    if (decompressed_size != uncompressed_size)
    {
        throw std::runtime_error("Mismatch in decompressed block size in first chunk.");
    }

    return decompressed_buffer;
}

std::vector<uint8_t> AbfParser::iterate_and_decompress_chunks_multithread(duckdb::FileHandle &file_handle_p, uint64_t &bytes_read, uint64_t datamodel_ofs, uint64_t datamodel_size, XPress9Wrapper &xpress9_wrapper, const MultiThreadMetadata &metadata, BackupLogHeader virtual_directory, const int trailing_chunks, uint64_t &skip_offset, uint32_t first_session_signature)
{
    // Calculate total number of chunks across all threads
    uint64_t total_head_chunks = metadata.head_thread_count * metadata.head_block_count;
    uint64_t total_tail_chunks = metadata.tail_thread_count * metadata.tail_block_count;
    uint64_t total_chunks = total_head_chunks + total_tail_chunks;

    std::vector<uint8_t> all_decompressed_data;
    uint64_t chunk_index = 0;
    uint32_t block_index_iterator = 0;

    // Process head/prefix chunks
    for (uint64_t i = 0; i < total_head_chunks && bytes_read < datamodel_size; ++i) {
        chunk_index++;
        
        // Read the compressed and uncompressed sizes
        uint32_t uncompressed_size = 0;
        uint32_t compressed_size = 0;
        file_handle_p.Read(reinterpret_cast<char *>(&uncompressed_size), sizeof(uncompressed_size));
        file_handle_p.Read(reinterpret_cast<char *>(&compressed_size), sizeof(compressed_size));
        bytes_read += sizeof(uncompressed_size) + sizeof(compressed_size);

        // Skip chunks if not within the last `trailing_chunks`
        if (total_chunks > static_cast<uint64_t>(trailing_chunks) && chunk_index < (total_chunks - trailing_chunks)) {
            skip_offset += uncompressed_size;
            bytes_read += compressed_size;
            file_handle_p.Seek(datamodel_ofs + bytes_read);
            continue;
        }

        // Allocate buffers for the compressed and decompressed data
        std::vector<uint8_t> compressed_buffer(compressed_size);
        std::vector<uint8_t> decompressed_buffer(uncompressed_size);

        // Read the compressed chunk
        file_handle_p.Read(reinterpret_cast<char *>(compressed_buffer.data()), compressed_size);
        bytes_read += compressed_size;

        // Patch header: update block_index and session_signature to match first chunk
        patch_header_of_compressed_buffer(compressed_buffer, block_index_iterator, first_session_signature);

        // Decompress the chunk
        uint32_t decompressed_size = xpress9_wrapper.Decompress(compressed_buffer.data(), compressed_size, decompressed_buffer.data(), decompressed_buffer.size());

        // Verify decompression success
        if (decompressed_size != uncompressed_size) {
            throw std::runtime_error("Decompression failed or resulted in unexpected size for head chunk.");
        }

        // Add decompressed data to the overall buffer
        all_decompressed_data.insert(all_decompressed_data.end(), decompressed_buffer.begin(), decompressed_buffer.end());
    }

    // Process tail/main chunks
    for (uint64_t i = 0; i < total_tail_chunks && bytes_read < datamodel_size; ++i) {
        chunk_index++;
        
        // Read the compressed and uncompressed sizes
        uint32_t uncompressed_size = 0;
        uint32_t compressed_size = 0;
        file_handle_p.Read(reinterpret_cast<char *>(&uncompressed_size), sizeof(uncompressed_size));
        file_handle_p.Read(reinterpret_cast<char *>(&compressed_size), sizeof(compressed_size));
        bytes_read += sizeof(uncompressed_size) + sizeof(compressed_size);

        // Skip chunks if not within the last `trailing_chunks`
        if (total_chunks > static_cast<uint64_t>(trailing_chunks) && chunk_index < (total_chunks - trailing_chunks)) {
            skip_offset += uncompressed_size;
            bytes_read += compressed_size;
            file_handle_p.Seek(datamodel_ofs + bytes_read);
            continue;
        }

        // Allocate buffers for the compressed and decompressed data
        std::vector<uint8_t> compressed_buffer(compressed_size);
        std::vector<uint8_t> decompressed_buffer(uncompressed_size);

        // Read the compressed chunk
        file_handle_p.Read(reinterpret_cast<char *>(compressed_buffer.data()), compressed_size);
        bytes_read += compressed_size;

        // Patch header: update block_index and session_signature to match first chunk
        patch_header_of_compressed_buffer(compressed_buffer, block_index_iterator, first_session_signature);

        // Decompress the chunk
        uint32_t decompressed_size = xpress9_wrapper.Decompress(compressed_buffer.data(), compressed_size, decompressed_buffer.data(), decompressed_buffer.size());

        // Verify decompression success
        if (decompressed_size != uncompressed_size) {
            throw std::runtime_error("Decompression failed or resulted in unexpected size for tail chunk.");
        }

        // Add decompressed data to the overall buffer
        all_decompressed_data.insert(all_decompressed_data.end(), decompressed_buffer.begin(), decompressed_buffer.end());
    }

    return all_decompressed_data;
}

DataModel AbfParser::get_sqlite(duckdb::ClientContext &context, const std::string &path, const int trailing_blocks)
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

    // Read compressed DataModel header to adjust offset
    file_handle->Seek(datamodel_ofs + ZIP_LOCAL_FILE_HEADER_FIXED);
    uint16_t filename_len = 0;
    uint16_t extra_len = 0;
    file_handle->Read(reinterpret_cast<char *>(&filename_len), sizeof(filename_len));
    file_handle->Read(reinterpret_cast<char *>(&extra_len), sizeof(extra_len));
    datamodel_ofs += ZIP_LOCAL_FILE_HEADER + filename_len + extra_len;

    // Detect the DataModel file type
    DataModelType file_type = DetectDataModelType(*file_handle, datamodel_ofs);

    // Throw exceptions for unsupported types
    if (file_type == DataModelType::UNCOMPRESSED)
    {
        throw std::runtime_error("Uncompressed ABF backup files are not yet supported. "
                                "Please use a compressed PBIX file (single-threaded or multi-threaded XPress9 compression).");
    }
    
    if (file_type == DataModelType::UNKNOWN)
    {
        throw std::runtime_error("Unknown or unsupported DataModel file format. "
                                "Expected one of: single-threaded XPress9, multi-threaded XPress9, or uncompressed ABF backup. "
                                "This file does not match any known signature.");
    }

    file_handle->Seek(datamodel_ofs);
    XPress9Wrapper xpress9_wrapper;
    if (!xpress9_wrapper.Initialize())
    {
        throw std::runtime_error("Failed to initialize XPress9Wrapper");
    }

    std::vector<uint8_t> initial_decompressed_buffer;
    BackupLogHeader backup_log_header;
    uint64_t skip_offset = 0;
    std::vector<uint8_t> all_decompressed_buffer;

    if (file_type == DataModelType::SINGLE_THREADED)
    {
        // Decompress initial block to get the virtual directory info
        initial_decompressed_buffer = decompress_initial_block(*file_handle, bytes_read, xpress9_wrapper);

        // Process backup log header to get virtual directory offset and size
        backup_log_header = process_backup_log_header(initial_decompressed_buffer);

        // Iterate through the remaining blocks and decompress them
        all_decompressed_buffer = iterate_and_decompress_blocks(*file_handle, bytes_read, datamodel_ofs, datamodel_size, xpress9_wrapper, backup_log_header, trailing_blocks, skip_offset);
    }
    else if (file_type == DataModelType::MULTI_THREADED)
    {
        // For multi-threaded files, read metadata first
        MultiThreadMetadata metadata;
        uint32_t first_session_signature = 0;
        initial_decompressed_buffer = decompress_initial_block_multithread(*file_handle, bytes_read, xpress9_wrapper, metadata, first_session_signature);

        // Process backup log header to get virtual directory offset and size
        backup_log_header = process_backup_log_header(initial_decompressed_buffer);

        // Iterate through the remaining chunks and decompress them
        all_decompressed_buffer = iterate_and_decompress_chunks_multithread(*file_handle, bytes_read, datamodel_ofs, datamodel_size, xpress9_wrapper, metadata, backup_log_header, trailing_blocks, skip_offset, first_session_signature);
    }

    // Prefix all_decompressed_buffer with initial_decompressed_buffer in case we have only one block/chunk
    all_decompressed_buffer.insert(all_decompressed_buffer.begin(), initial_decompressed_buffer.begin(), initial_decompressed_buffer.end());

    if (skip_offset + all_decompressed_buffer.size() < backup_log_header.m_cbOffsetHeader + backup_log_header.DataSize)
    {
        throw std::runtime_error("Could not parse the entire DataModel.");
    }

    // Finally, extract the SQLite buffer from the decompressed data
    return extract_sqlite_buffer(all_decompressed_buffer, skip_offset, backup_log_header);
}
#include "AbfParser.h"

using namespace tinyxml2;

std::vector<uint8_t> AbfParser::read_buffer_bytes(const std::vector<uint8_t> &buffer, uint64_t offset, int size)
{
    if (offset < 0 || size < 0 || static_cast<size_t>(offset) + size > buffer.size()) {
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


std::vector<uint8_t> AbfParser::patch_header(const pbix_t::chunk_t* chunk, uint32_t& block_index_iterator) {

    // Increment the block index iterator as part of the patching process
    block_index_iterator++;

    // Extract the header from the chunk, ensuring non-null pointers
    auto header = chunk->node()->header();
    if (header == nullptr) {
        throw std::runtime_error("Chunk's node header is null");
    }

    auto magic = header->xpress_magic();
    std::vector<uint8_t> header_buffer(magic.begin(), magic.end());
    // Serialize it back to a buffer
    // UInt32 serialization routine
    auto write_uint32 = [&header_buffer](uint32_t value) {
        for (int i = 0; i < 4; ++i) {
            header_buffer.push_back((value >> (i * 8)) & 0xFF);
        }
    };
    // Serialize header fields into the buffer
    write_uint32(header->orig_size());
    write_uint32(header->encoded_size());

    // Insert Huffman table flags and other header components
    auto huff = header->_raw_huffman_table_flags();
    header_buffer.insert(header_buffer.end(), huff.begin(), huff.end());
    
    write_uint32(header->zero());
    write_uint32(header->session_signature());
    write_uint32(block_index_iterator);

    auto crc32 = crc32c::Crc32c(header_buffer.data(), header_buffer.size());
    write_uint32(crc32);

    // Append the segment data
    const auto& segments = chunk->node()->segments();
    header_buffer.insert(header_buffer.end(), segments.begin(), segments.end());

    return std::vector<uint8_t>(header_buffer.begin(), header_buffer.end());
}

std::vector<uint8_t> AbfParser::process_chunk(const pbix_t::chunk_t* chunk, uint32_t block_index, uint32_t& block_index_iterator) {
    if (block_index != block_index_iterator)
    {
        // Patch the header with a new block index
        return patch_header(chunk, block_index_iterator);
    }
    const auto& node = chunk->_raw_node();
    return std::vector<unsigned char>(node.begin(), node.end());

}

std::tuple<uint64_t,int> AbfParser::process_backup_log_header(const std::vector<uint8_t> &buffer)
{
    constexpr int backup_log_header_offset = 72;
    constexpr int backup_log_header_size = 0x1000 - backup_log_header_offset;

    std::vector<uint8_t> backup_log_header_buffer = read_buffer_bytes(buffer, backup_log_header_offset, backup_log_header_size);
    backup_log_header_buffer = trim_buffer(backup_log_header_buffer);
    BackupLogHeader backup_log_header = BackupLogHeader::from_xml(backup_log_header_buffer, "UTF-16");
    return {backup_log_header.m_cbOffsetHeader, backup_log_header.DataSize};
}

std::vector<uint8_t> AbfParser::extract_sqlite_buffer(const std::vector<uint8_t> &buffer, uint64_t skip_offset, uint64_t virtual_directory_offset, int virtual_directory_size)
{
    std::string buffer_str(buffer.begin(), buffer.end());
    std::vector<uint8_t> virtual_directory_buffer = read_buffer_bytes(buffer, virtual_directory_offset - skip_offset, virtual_directory_size);
    VirtualDirectory virtual_directory = VirtualDirectory::from_xml(virtual_directory_buffer, "UTF-8");

    // Validate the presence of backup files before proceeding.
    if (virtual_directory.backupFiles->empty()) {
        throw std::runtime_error("No backup files found in virtual directory.");
    }

    const BackupFile& log = virtual_directory.backupFiles->back();
    uint64_t backup_file_offset = log.m_cbOffsetHeader;
    int backup_file_size = log.Size;

    std::vector<uint8_t> backup_log_buffer = read_buffer_bytes(buffer, backup_file_offset + 2 - skip_offset, backup_file_size - 2);
    BackupLog backup_log = BackupLog::from_xml(backup_log_buffer, "UTF-16");

    // Ensure there are enough backup files to safely access the second to last one.
    if (virtual_directory.backupFiles->size() < 2) {
        throw std::runtime_error("Insufficient backup files for SQLite extraction.");
    }

    auto sqlite = (*virtual_directory.backupFiles)[virtual_directory.backupFiles->size() - 2];
    uint64_t sqlite_offset = sqlite.m_cbOffsetHeader;
    int sqlite_size = sqlite.Size;

    return read_buffer_bytes(buffer, sqlite_offset - skip_offset, sqlite_size);
}

std::vector<uint8_t> AbfParser::get_sqlite(const std::string &path, const int trailing_chunks=15)
{
    constexpr auto DataModelFileName = "DataModel";
    constexpr uint32_t LocalFileType = 1027;

    int virtual_directory_offset = 0;
    int virtual_directory_size = 0;
    uint64_t skip_offset = 0;
    uint32_t block_index_iterator = 0;

    std::ifstream is_pbix(path, std::ifstream::binary);
    if (!is_pbix.is_open())
    {
        throw std::runtime_error("Failed to open PBIX file");
    }
    kaitai::kstream ks_pbix(&is_pbix);
    pbix_t zip_archive(&ks_pbix);

    // Loop through each section in the ZIP file
    for (const auto& section : *zip_archive.sections())
    {
        if (section->section_type() != LocalFileType) continue;

        auto localFile = static_cast<pbix_t::local_file_t *>(section->body());
        if (localFile->header()->file_name() != DataModelFileName) continue;

        //initialize the decompressed data buffer and the XPress9Wrapper
        XPress9Wrapper xpress9_wrapper;
        if (!xpress9_wrapper.Initialize())
        {
            throw std::runtime_error("Failed to initialize XPress9Wrapper");
        }
        //used to asses if we need to skip chunks as it's a big file
        auto total_chunks = localFile->datamodel()->chunks()->size();
        std::vector<uint8_t> all_decompressed_data;

        for (const auto& chunk : *localFile->datamodel()->chunks())
        {

            
            int block_index = chunk->node()->header()->block_index();
            if (total_chunks > trailing_chunks && !(block_index == 0 || block_index >= total_chunks - trailing_chunks))
            {
                skip_offset += chunk->uncompressed();
                continue;
            }

            std::vector<uint8_t> compressed_data = process_chunk(chunk, block_index, block_index_iterator); // Process chunks.

            // Buffers for storing decompressed data
            std::vector<uint8_t> decompressed_buffer(chunk->uncompressed());

            // Decompress the entire data
            uint32_t decompressed_size = xpress9_wrapper.Decompress(compressed_data.data(), chunk->compressed(), decompressed_buffer.data(), decompressed_buffer.size());
            
            // Verify that the total decompressed size matches the expected size
            if (decompressed_size != chunk->uncompressed()) {
                throw std::runtime_error("Mismatch in decompressed chunk size.");
            }
            
            // Add the decompressed data to the overall buffer
            all_decompressed_data.insert(all_decompressed_data.end(), decompressed_buffer.begin(), decompressed_buffer.end());
            
            uint64_t virtual_directory_offset;
            int virtual_directory_size;
            if (block_index == 0) {
                std::tie(virtual_directory_offset, virtual_directory_size) = process_backup_log_header(all_decompressed_data); // Processes the backup log header.
            }

            if (skip_offset + all_decompressed_data.size() >= virtual_directory_offset + virtual_directory_size)
            {

                return extract_sqlite_buffer(all_decompressed_data, skip_offset, virtual_directory_offset, virtual_directory_size); // Extracts and returns the SQLite buffer.
            }
        }
    }
    throw std::runtime_error("DataModel metadata not found in PBIX file.");
}
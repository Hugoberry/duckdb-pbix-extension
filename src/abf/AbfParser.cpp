#include "AbfParser.h"

using namespace tinyxml2;

std::vector<uint8_t> AbfParser::read_buffer_bytes(const std::vector<uint8_t> &buffer, long offset, int size)
{
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

    // Assuming block_index and block_index_iterator are maintained outside this function
    block_index_iterator++;
    // Make a copy of the header structure
    auto header = chunk->node()->header();

    auto magic = header->xpress_magic();
    std::vector<uint8_t> header_buffer(magic.begin(), magic.end());
    // Serialize it back to a buffer
    // UInt32 serialization routine
    auto write_uint32 = [&](uint32_t value) {
        header_buffer.push_back(value & 0xFF);
        header_buffer.push_back((value >> 8) & 0xFF);
        header_buffer.push_back((value >> 16) & 0xFF);
        header_buffer.push_back((value >> 24) & 0xFF);
    };
    // header_buffer.insert(header_buffer.end(), magic.begin(), magic.end());
    write_uint32(header->orig_size());
    write_uint32(header->encoded_size());
    auto huff = header->_raw_huffman_table_flags();
    header_buffer.insert(header_buffer.end(), huff.begin(), huff.end());
    write_uint32(header->zero());
    write_uint32(header->session_signature());
    write_uint32(block_index_iterator);
    auto crc32 = crc32c::Crc32c(header_buffer.data(), header_buffer.size());
    write_uint32(crc32);
    auto segments = chunk->node()->segments();

    header_buffer.insert(header_buffer.end(), segments.begin(), segments.end());

    return std::vector<uint8_t>(header_buffer.begin(), header_buffer.end());
}

std::vector<uint8_t> AbfParser::process_chunk(const pbix_t::chunk_t* chunk, uint32_t block_index, uint32_t& block_index_iterator) {
    if (block_index != block_index_iterator)
    {
        // Patch the header with a new block index
        return patch_header(chunk, block_index_iterator);
    }
    auto node = chunk->_raw_node();
    return std::vector<unsigned char>(node.begin(), node.end());

}

std::tuple<int,int> AbfParser::process_backup_log_header(const std::vector<uint8_t> &buffer)
{
    constexpr int backup_log_header_offset = 72;
    constexpr int backup_log_header_size = 0x1000 - backup_log_header_offset;

    std::vector<uint8_t> backup_log_header_buffer = read_buffer_bytes(buffer, backup_log_header_offset, backup_log_header_size);
    backup_log_header_buffer = trim_buffer(backup_log_header_buffer);
    BackupLogHeader backup_log_header = BackupLogHeader::from_xml(backup_log_header_buffer, "UTF-16");
    return std::make_tuple(static_cast<int>(backup_log_header.m_cbOffsetHeader), static_cast<int>(backup_log_header.DataSize));
}

std::vector<uint8_t> AbfParser::extract_sqlite_buffer(const std::vector<uint8_t> &buffer, int skip_offset, int virtual_directory_offset, int virtual_directory_size)
{
    std::string buffer_str(buffer.begin(), buffer.end());
    std::vector<uint8_t> virtual_directory_buffer = read_buffer_bytes(buffer, virtual_directory_offset - skip_offset, virtual_directory_size);
    VirtualDirectory virtual_directory = VirtualDirectory::from_xml(virtual_directory_buffer, "UTF-8");

    BackupFile log = virtual_directory.backupFiles->back();
    int backup_file_offset = static_cast<int>(log.m_cbOffsetHeader);
    int backup_file_size = static_cast<int>(log.Size);

    std::vector<uint8_t> backup_log_buffer = read_buffer_bytes(buffer, backup_file_offset + 2 - skip_offset, backup_file_size - 2);
    BackupLog backup_log = BackupLog::from_xml(backup_log_buffer, "UTF-16");

    auto sqlite = (*virtual_directory.backupFiles)[static_cast<int>(virtual_directory.backupFiles->size()) - 2];
    int sqlite_offset = static_cast<int>(sqlite.m_cbOffsetHeader);
    int sqlite_size = static_cast<int>(sqlite.Size);

    return read_buffer_bytes(buffer, sqlite_offset - skip_offset, sqlite_size);
    
}
std::vector<uint8_t> AbfParser::get_sqlite(const std::string &path)
{

    constexpr int trailing_chunks = 15;
    constexpr auto DataModelFileName = "DataModel";
    constexpr uint32_t LocalFileType = 1027;

    int virtual_directory_offset = 0;
    int virtual_directory_size = 0;
    int skip_offset = 0;
    uint32_t block_index_iterator = 0;

    std::ifstream is_pbix(path, std::ifstream::binary);
    if (!is_pbix.is_open())
    {
        throw std::runtime_error("Failed to open PBIX file");
    }
    kaitai::kstream ks_pbix(&is_pbix);
    pbix_t zip_archive(&ks_pbix);

    // Loop through each section in the ZIP file
    for (auto &section : *zip_archive.sections())
    {
        if (section->section_type() != LocalFileType) continue;

        auto localFile = static_cast<pbix_t::local_file_t *>(section->body());
        if (localFile->header()->file_name() != DataModelFileName) continue;

        //initialize the decompressed data buffer and the XPress9Wrapper
        std::vector<uint8_t> all_decompressed_data;
        XPress9Wrapper xpress9_wrapper;
        if (!xpress9_wrapper.Initialize())
        {
            throw std::runtime_error("Failed to initialize XPress9Wrapper");
        }
        //used to asses if we need to skip chunks as it's a big file
        auto total_chunks = localFile->datamodel()->chunks()->size();

        for (auto &chunk : *localFile->datamodel()->chunks())
        {

            
            int block_index = chunk->node()->header()->block_index();
            if (total_chunks > trailing_chunks && !(block_index == 0 || block_index >= total_chunks - trailing_chunks))
            {
                skip_offset += chunk->uncompressed();
                continue;
            }

            std::vector<uint8_t> compressed_data = process_chunk(chunk, block_index, block_index_iterator); // Process chunks.

            // Buffers for storing decompressed data
            std::vector<uint8_t> x9_decompressed_buffer(chunk->uncompressed());

            // Decompress the entire data
            uint32_t totalDecompressedSize = xpress9_wrapper.Decompress(compressed_data.data(), chunk->compressed(), x9_decompressed_buffer.data(), x9_decompressed_buffer.size());
            
            // Verify that the total decompressed size matches the expected size
            if (totalDecompressedSize != chunk->uncompressed())
            {
                throw std::runtime_error("Decompressed size does not match expected size.");
            }
            
            // Add the decompressed data to the overall buffer
            all_decompressed_data.insert(all_decompressed_data.end(), x9_decompressed_buffer.begin(), x9_decompressed_buffer.end());
            
            int virtual_directory_offset, virtual_directory_size;
            if (block_index == 0) {
                std::tie(virtual_directory_offset, virtual_directory_size) = process_backup_log_header(all_decompressed_data); // Processes the backup log header.
            }

            if (skip_offset + all_decompressed_data.size() >= virtual_directory_offset + virtual_directory_size)
            {

                return extract_sqlite_buffer(all_decompressed_data, skip_offset, virtual_directory_offset, virtual_directory_size); // Extracts and returns the SQLite buffer.
            }
        }
    }
}
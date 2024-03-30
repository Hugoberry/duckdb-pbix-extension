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


std::tuple<uint64_t,int> AbfParser::process_backup_log_header(const std::vector<uint8_t> &buffer)
{
    constexpr int backup_log_header_offset = 72;
    constexpr int backup_log_header_size = 0x1000 - backup_log_header_offset;

    std::vector<uint8_t> backup_log_header_buffer = read_buffer_bytes(buffer, backup_log_header_offset, backup_log_header_size);
    backup_log_header_buffer = trim_buffer(backup_log_header_buffer);
    std::cout << "Backup log header: " << std::string(backup_log_header_buffer.begin(), backup_log_header_buffer.end()) << std::endl;
    BackupLogHeader backup_log_header = BackupLogHeader::from_xml(backup_log_header_buffer, "UTF-16");
    return {backup_log_header.m_cbOffsetHeader, backup_log_header.DataSize};
}

std::vector<uint8_t> AbfParser::extract_sqlite_buffer(const std::vector<uint8_t> &buffer, uint64_t skip_offset, uint64_t virtual_directory_offset, int virtual_directory_size)
{
    std::cout << "Extracting SQLite buffer..." << std::endl;
    std::cout << "Skip offset: " << skip_offset << std::endl;
    std::cout << "Virtual directory offset: " << virtual_directory_offset << std::endl;
    std::cout << "Virtual directory size: " << virtual_directory_size << std::endl;
    // std::cout << "Backup log header: " << std::string(buffer.begin(), buffer.end()+200) << std::endl;
    
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

void AbfParser::patch_header_of_compressed_buffer(std::vector<uint8_t> &compressed_buffer, uint32_t& block_index_iterator)
{
    block_index_iterator++;

    Header header = Header::extract_from_buffer(compressed_buffer, 0);
    header.block_index = block_index_iterator;
    header.update_crc();
    header.insert_into_buffer(compressed_buffer, 0);

}

std::vector<uint8_t> AbfParser::get_sqlite(const std::string &path, const int trailing_chunks=15)
{
    constexpr auto DataModelFileName = "DataModel";
    constexpr uint32_t LocalFileType = 1027;

    int virtual_directory_offset = 0;
    int virtual_directory_size = 0;
    uint64_t skip_offset = 0;
    uint32_t block_index_iterator = 0;
    uint32_t block_index = 0;

    mz_zip_archive zip_archive;
    uint64_t datamodel_ofs = 0;
    uint64_t datamodel_size = 0;
    memset(&zip_archive, 0, sizeof(zip_archive));
    mz_bool status = mz_zip_reader_init_file(&zip_archive, path.c_str(), 0);
    if (!status) {
        throw std::runtime_error("Could not open pbix file");
    }
    int file_index = mz_zip_reader_locate_file(&zip_archive, DataModelFileName, NULL, 0);
    if (file_index < 0) {
        throw std::runtime_error("DataModel not found in the pbix file.");
    } else {
        mz_zip_archive_file_stat file_stat;
        if (mz_zip_reader_file_stat(&zip_archive, file_index, &file_stat)) {
            datamodel_size=  file_stat.m_comp_size;
            datamodel_ofs = file_stat.m_local_header_ofs;

        } else {
            throw std::runtime_error("Could not retrieve information about DataModel.");
        }
    }
    mz_zip_reader_end(&zip_archive);
    std::ifstream entryStream(path, std::ios::binary);
    if (!entryStream.is_open()) {
        throw std::runtime_error("Could not open pbix file");
    }
    entryStream.seekg(datamodel_ofs+26);
    uint16_t filename_len = 0;
    uint16_t extra_len = 0;
    entryStream.read(reinterpret_cast<char *>(&filename_len), sizeof(filename_len));
    entryStream.read(reinterpret_cast<char *>(&extra_len), sizeof(extra_len));

    datamodel_ofs += 30+filename_len+extra_len;
    entryStream.seekg(datamodel_ofs+102);

    uint32_t uncompressed_size;
    uint32_t compressed_size;

    //process the first chunk
    // Read the compressed and uncompressed sizes before the offset
    entryStream.read(reinterpret_cast<char*>(&uncompressed_size), sizeof(uint32_t));
    entryStream.read(reinterpret_cast<char*>(&compressed_size), sizeof(uint32_t));

    XPress9Wrapper xpress9_wrapper;
    if (!xpress9_wrapper.Initialize())
    {
        throw std::runtime_error("Failed to initialize XPress9Wrapper");
    }

    // Buffers for storing decompressed data
    std::vector<uint8_t> c0_decompressed_buffer(uncompressed_size);
    std::vector<uint8_t> c0_compressed_buffer(compressed_size);
    std::vector<uint8_t> all_decompressed_buffer;

    entryStream.read(reinterpret_cast<char*>(c0_compressed_buffer.data()), compressed_size);

    // Decompress the entire data
    uint32_t decompressed_size = xpress9_wrapper.Decompress(c0_compressed_buffer.data(), compressed_size, c0_decompressed_buffer.data(), c0_decompressed_buffer.size());
    // Verify that the total decompressed size matches the expected size
    if (decompressed_size != uncompressed_size) {
        throw std::runtime_error("Mismatch in decompressed chunk size in first chunk.");
    }
    all_decompressed_buffer.insert(all_decompressed_buffer.end(), c0_decompressed_buffer.begin(), c0_decompressed_buffer.end()); // Add the decompressed data to the overall buffer

    std::tie(virtual_directory_offset, virtual_directory_size) = process_backup_log_header(all_decompressed_buffer);

    auto total_chunks = (virtual_directory_size + virtual_directory_offset) / 0x200000;
    skip_offset += uncompressed_size;

    while(entryStream.tellg()<datamodel_ofs+datamodel_size)
    {
        block_index++;
        // Read the compressed and uncompressed sizes before the offset
        entryStream.read(reinterpret_cast<char*>(&uncompressed_size), sizeof(uint32_t));
        entryStream.read(reinterpret_cast<char*>(&compressed_size), sizeof(uint32_t));
        if (total_chunks > trailing_chunks && !(block_index >= total_chunks - trailing_chunks))
        {
            skip_offset += uncompressed_size;
            entryStream.seekg(compressed_size, std::ios::cur);
            continue;
        }
        // Buffers for storing decompressed data
        std::vector<uint8_t> decompressed_buffer(uncompressed_size);
        std::vector<uint8_t> compressed_buffer(compressed_size);
        std::cout << "Block index: " << block_index << std::endl;
        entryStream.read(reinterpret_cast<char*>(compressed_buffer.data()), compressed_size);
        // call to a new function process header_buffer which we'll use to modify compressed_buffer
        patch_header_of_compressed_buffer(compressed_buffer, block_index_iterator);

        decompressed_size = xpress9_wrapper.Decompress(compressed_buffer.data(), compressed_size, decompressed_buffer.data(), decompressed_buffer.size());
        // Verify that the total decompressed size matches the expected size
        if (decompressed_size != uncompressed_size) {
            throw std::runtime_error("Mismatch in decompressed chunk size.");
        }
        all_decompressed_buffer.insert(all_decompressed_buffer.end(), decompressed_buffer.begin(), decompressed_buffer.end()); // Add the decompressed data to the overall buffer

        if (skip_offset + all_decompressed_buffer.size() >= virtual_directory_offset + virtual_directory_size)
        {
            return extract_sqlite_buffer(all_decompressed_buffer, skip_offset, virtual_directory_offset, virtual_directory_size); // Extracts and returns the SQLite buffer.
        }
    }

    throw std::runtime_error("DataModel metadata not found in PBIX file.");
}
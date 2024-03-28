#include "AbfParser.h"
#include <fstream>
// tinyxml and sqlite3 linked through vcpkg


using namespace tinyxml2;

std::vector<uint8_t> AbfParser::read_buffer_bytes(const std::vector<uint8_t>& buffer, long offset, int size) {
    std::vector<uint8_t> sub_buffer(size);
    std::copy(buffer.begin() + offset, buffer.begin() + offset + size, sub_buffer.begin());
    return sub_buffer;
}

std::vector<uint8_t> AbfParser::trim_buffer(const std::vector<uint8_t>& buffer) {
    int last_non_zero_byte = buffer.size() - 1;
    while (last_non_zero_byte >= 1 && buffer[last_non_zero_byte] == 0x00 && buffer[last_non_zero_byte - 1] == 0x00) {
        last_non_zero_byte -= 2;
    }
    last_non_zero_byte++;
    std::vector<uint8_t> trimmed_buffer(buffer.begin(), buffer.begin() + last_non_zero_byte + 1);
    return trimmed_buffer;
}

std::vector<uint8_t> AbfParser::get_sqlite(const std::string &path) {
    int offset = 72;
    int page = 0x1000;
    bool parsed_backup_log_header = false;
    int virtual_directory_offset=0;
    int virtual_directory_size=0;
    int abf_currsor = 0;
    uint32_t block_index_iterator = 0;

    std::ifstream is_pbix(path, std::ifstream::binary);
    if (!is_pbix.is_open()) {
        throw std::runtime_error("Failed to open PBIX file");
    }
    kaitai::kstream ks_pbix(&is_pbix);
    pbix_t data(&ks_pbix);

    // Loop through each section in the ZIP file
    for (auto& section : *data.sections()) {
        if (section->section_type() != 1027) { // Not a LOCAL_FILE, skip it
            continue;
        }
        auto localFile = static_cast<pbix_t::local_file_t*>(section->body());
        if (localFile->header()->file_name() != "DataModel") { // Not "DataModel", skip it
            continue;
        }

        // If we reach here, we have found the DataModel section
        auto datamodel = localFile->datamodel();
        
        std::vector<uint8_t> allDecompressedData;
        XPress9Wrapper xpress9Wrapper;
        if (!xpress9Wrapper.Initialize()) {
            throw std::runtime_error("Failed to initialize XPress9Wrapper");
        }

        auto chunk_count = datamodel->chunks()->size();

        for (auto& chunk : *datamodel->chunks()) {

            if(chunk->node()->header()->block_index()>0 && chunk->node()->header()->block_index() < chunk_count-20){
                continue;
            }
            std::vector<uint8_t> compressedData;
            if(chunk->node()->header()->block_index() != block_index_iterator){
                block_index_iterator++;
                //make a copy of the header structure
                auto header = chunk->node()->header();
                //searialize it back to a buffer
               
                std::vector<uint8_t> header_buffer;
                // Example serialization code
                auto write_uint32 = [&](uint32_t value) {
                    header_buffer.push_back(value & 0xFF);
                    header_buffer.push_back((value >> 8) & 0xFF);
                    header_buffer.push_back((value >> 16) & 0xFF);
                    header_buffer.push_back((value >> 24) & 0xFF);
                };
                auto magic = header->xpress_magic();

                header_buffer.insert(header_buffer.end(), magic.begin(), magic.end());

                // std::copy(header->xpress_magic().begin(), header->xpress_magic().end(), std::back_inserter(header_buffer));

                // header_buffer.push_back(header->orig_size());
                write_uint32(header->orig_size());
                write_uint32(header->encoded_size());

                auto huff = header->_raw_huffman_table_flags();

                header_buffer.insert(header_buffer.end(), huff.begin(), huff.end());
                // std::copy(header->_raw_huffman_table_flags().begin(), header->_raw_huffman_table_flags().end(), std::back_inserter(header_buffer));
                write_uint32(header->zero());
                write_uint32(header->session_signature());
                write_uint32(block_index_iterator);

                std::cout << std::hex << std::showbase<< std::uppercase;

                std::cout << "block_index_iterator: " << block_index_iterator << std::endl;
                std::cout << "header->block_index(): " << header->block_index() << std::endl;
                std::cout << "header->crc32(): " << header->crc32() << std::endl;
                auto crc32 = crc32c::Crc32c(header_buffer.data(), header_buffer.size());
                write_uint32(crc32);
                std::cout << "crc32: " << crc32 << std::endl;
                std::cout << "header " << header << std::endl;
                // std::cout << "header_buffer " << header_buffer << std::endl;
                auto segments = chunk->node()->segments();

                header_buffer.insert(header_buffer.end(), segments.begin(), segments.end());
                // std::copy(chunk->node()->segments().begin(), chunk->node()->segments().end(), std::back_inserter(header_buffer));

                compressedData =  std::vector<unsigned char>(header_buffer.begin(), header_buffer.end());    
            } else {
                auto node = chunk->_raw_node();
                compressedData =  std::vector<unsigned char>(node.begin(), node.end());    
            }
            // Buffers for storing decompressed data
            std::vector<uint8_t> x9DecompressedBuffer(chunk->uncompressed());


            // Decompress the entire data
            uint32_t totalDecompressedSize = xpress9Wrapper.Decompress(compressedData.data(), chunk->compressed(), x9DecompressedBuffer.data(), x9DecompressedBuffer.size());

            // Verify that the total decompressed size matches the expected size
            if (totalDecompressedSize != chunk->uncompressed()) {
                throw std::runtime_error("Decompressed size does not match expected size.");
            }
            else {
                // Add the decompressed data to the overall buffer
                allDecompressedData.insert(allDecompressedData.end(), x9DecompressedBuffer.begin(), x9DecompressedBuffer.end());
            }

            if(!parsed_backup_log_header){
                std::vector<uint8_t> backup_log_header_buffer = read_buffer_bytes(allDecompressedData, offset, page - offset);
                backup_log_header_buffer = trim_buffer(backup_log_header_buffer);
                BackupLogHeader backup_log_header = BackupLogHeader::from_xml(backup_log_header_buffer, "UTF-16");
                virtual_directory_offset = static_cast<int>(backup_log_header.m_cbOffsetHeader);
                virtual_directory_size = static_cast<int>(backup_log_header.DataSize);
                parsed_backup_log_header = true;
            }
            
            if(abf_currsor + totalDecompressedSize >= virtual_directory_offset + virtual_directory_size){
                std::vector<uint8_t> virtual_directory_buffer = read_buffer_bytes(allDecompressedData, virtual_directory_offset, virtual_directory_size);
                VirtualDirectory virtual_directory = VirtualDirectory::from_xml(virtual_directory_buffer, "UTF-8");

                BackupFile log = virtual_directory.backupFiles->back();
                int backup_file_offset = static_cast<int>(log.m_cbOffsetHeader);
                int backup_file_size = static_cast<int>(log.Size);
                std::vector<uint8_t> backup_log_buffer = read_buffer_bytes(allDecompressedData, backup_file_offset+2, backup_file_size-2);
                BackupLog backup_log = BackupLog::from_xml(backup_log_buffer, "UTF-16");

                auto sqlite = (*virtual_directory.backupFiles)[static_cast<int>(virtual_directory.backupFiles->size()) - 2];
                int sqlite_offset = static_cast<int>(sqlite.m_cbOffsetHeader);
                int sqlite_size = static_cast<int>(sqlite.Size);
                std::vector<uint8_t> sqlite_buffer = read_buffer_bytes(allDecompressedData, sqlite_offset, sqlite_size);

                return sqlite_buffer;
            }
            abf_currsor += chunk->uncompressed();
        }

    }
}
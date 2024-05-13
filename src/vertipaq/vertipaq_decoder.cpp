#include "vertipaq_decoder.hpp"
#include <iostream>
#include "huffman_decoder.h"
namespace duckdb
{
    std::vector<uint64_t> VertipaqDecoder::readBitPacked(const std::vector<uint64_t> &sub_segment, uint64_t bit_width, uint64_t min_data_id)
    {
        uint64_t mask = (1ULL << bit_width) - 1;
        std::vector<uint64_t> res;
        for (auto u8le : sub_segment)
        {
            for (int i = 0; i < 64 / bit_width; i++)
            {
                res.push_back(min_data_id + (u8le & mask));
                u8le >>= bit_width;
            }
        }
        return res;
    }

    std::map<uint64_t, std::string> VertipaqDecoder::readDictionary(std::string &dictionary_stream, uint64_t min_data_id)
    {

        std::istringstream is_d(dictionary_stream);
        kaitai::kstream ks_d(&is_d);
        column_data_dictionary_t dictionary(&ks_d);

        // Checking dictionary type and processing accordingly
        if (dictionary.dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_STRING)
        {
            // Handling string-based dictionary
            std::map<uint64_t, std::string> hashtable;
            int index = min_data_id;

            auto stringData = static_cast<column_data_dictionary_t::string_data_t*>(dictionary.data());
            auto pages = stringData->dictionary_pages();
            auto record_handles = stringData->dictionary_record_handles_vector_info()->vector_of_record_handle_structures();
            std::unordered_map<int, std::vector<int>> record_handles_map;

            // make record_handle a map of page_id and bit_or_byte_offset
            for (const auto& handle : *record_handles) {
                record_handles_map[handle->page_id()].push_back(handle->bit_or_byte_offset());
            }
            
            for(int page_id = 0; page_id < pages->size(); page_id++){
                const auto& page = pages->at(page_id);
                auto it = record_handles_map.find(page_id);
                if (it != record_handles_map.end()) {
                    if(page->page_compressed()){
                        auto compressed_store = static_cast<column_data_dictionary_t::compressed_strings_t*>(page->string_store());
                        auto encode_array = compressed_store->encode_array();
                        auto store_total_bits = compressed_store->store_total_bits();
                        auto compressed_string_buffer = compressed_store->compressed_string_buffer();
                        auto ui_decode_bits = compressed_store->ui_decode_bits();

                        auto full_encode_array = decompress_encode_array(*encode_array);

                        HuffmanTree* huffman_tree = build_huffman_tree(full_encode_array);

                        for (size_t i = 0; i < it->second.size(); i++) {
                            uint32_t start_bit = it->second[i];
                            uint32_t end_bit = (i + 1 < it->second.size()) ? it->second[i + 1] : store_total_bits; // end of the compressed buffer
                            std::string decompressed = decode_substring(compressed_string_buffer, huffman_tree, start_bit, end_bit);

                            hashtable[index++] = decompressed;
                        }
                        delete huffman_tree;
                    } else {
                        auto uncompressed_store = static_cast<column_data_dictionary_t::uncompressed_strings_t *>(page->string_store());
                        auto buffer_used_characters = uncompressed_store->buffer_used_characters();
                        auto uncompressed = uncompressed_store->uncompressed_character_buffer();
                        std::cout << "Uncompressed buffer size: " << buffer_used_characters << std::endl;
                        std::cout << "Uncompressed buffer: " << uncompressed << std::endl;
                        for (size_t i = 0; i < it->second.size(); i++) {
                            uint32_t start_byte = it->second[i];
                            uint32_t end_byte = (i + 1 < it->second.size()) ? it->second[i + 1] : buffer_used_characters; // end of the uncompressed buffer
                            std::string str(uncompressed.begin() + start_byte, uncompressed.begin() + end_byte - 1);
                            auto ws= uncompressed.substr(start_byte, end_byte - start_byte+1 );
                            std::cout << ws << "|"<< start_byte << "|" << end_byte << "][";
                            hashtable[index++] = str;
                        }
                        std::cout << std::endl;
                        // Extracting strings from the uncompressed buffer
                        // auto char_buffer = reinterpret_cast<const char *>(uncompressed);
                        // while (*char_buffer != '\0')
                        // {
                        //     std::string str(char_buffer);
                        //     hashtable[index++] = str;
                        //     char_buffer += str.size() + 1;
                        // }
                        // auto wchar_buffer = reinterpret_cast<const wchar_t*>(uncompressed);
                        // while(*wchar_buffer != L'\0'){
                        //     std::wstring ws(wchar_buffer);
                        //     std::string str(ws.begin(), ws.end());
                        //     hashtable[index++] = str;
                        //     wchar_buffer += str.size() + 1;
                        // }
                        // std::istringstream ss(uncompressed);
                        // std::string token;
                        // while (std::getline(ss, token, L'\0'))
                        // { // assuming null-terminated strings in buffer
                        //     hashtable[index++] = token;
                        // }
                    }
                }

            } // end loop all pages
            return hashtable;
        }
        else if (dictionary.dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_LONG ||
                 dictionary.dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_REAL)
        {
            // Handling numeric data
            std::map<uint64_t, double> values;
            auto numberData = static_cast<column_data_dictionary_t::number_data_t *>(dictionary.data());
            auto vector_info = numberData->vector_of_vectors_info();
            const auto &vals = vector_info->values();
            int index = min_data_id;
            for (double val : *vals)
            {
                values[index++] = val;
            }

            // Converting to string map for a unified interface (if needed)
            std::map<uint64_t, std::string> str_values;
            for (const auto &pair : values)
            {
                str_values[pair.first] = std::to_string(pair.second);
            }
            return str_values;
        }
    }

    std::vector<uint64_t> VertipaqDecoder::readRLEBitPackedHybrid(std::string &idf_stream, uint64_t entries, uint64_t min_data_id, uint64_t bit_width)
    {
        std::istringstream stream(idf_stream);
        kaitai::kstream ks_idf(&stream);
        // auto result = readRLEBitPackedHybrid(ks_idf, idf_m.count_bit_packed, idf_m.min_data_id, idf_m.bit_width);
        column_data_idf_t column_data(&ks_idf);

        std::vector<uint64_t> bitpacked_values;
        std::vector<uint64_t> vector;
        uint64_t bit_packed_entries = 0;
        uint64_t bit_packed_offset = 0;

        if (entries > 0 && !column_data.segments()->empty())
        {
            auto &segments = *column_data.segments();
            auto segment = segments[0]; // Assuming there is at least one segment
            uint64_t size = segment->sub_segment_size();
            if (segment->sub_segment()->back() == 0 && size == 1)
            {
                bitpacked_values.assign(entries, min_data_id);
            }
            else
            {
                // Assuming `sub_segment` is accessible and contains meaningful data
                const auto &sub_segment = *segment->sub_segment();
                bitpacked_values = readBitPacked(sub_segment, bit_width, min_data_id);
            }
        }

        if (!column_data.segments()->empty())
        {
            const auto &primary_segment = *column_data.segments()->at(0)->primary_segment();
            for (const auto &entry : primary_segment)
            {
                if (entry->data_value() + bit_packed_offset == 0xFFFFFFFF)
                { // bit pack marker
                    bit_packed_entries = entry->repeat_value();
                    auto begin_it = bitpacked_values.begin() + bit_packed_offset;
                    auto end_it = begin_it + bit_packed_entries;
                    vector.insert(vector.end(), begin_it, end_it);
                    bit_packed_offset += bit_packed_entries;
                }
                else
                {
                    std::vector<uint64_t> rle(entry->repeat_value(), entry->data_value());
                    vector.insert(vector.end(), rle.begin(), rle.end());
                }
            }
        }

        return vector;
    }

    IdfMetadata VertipaqDecoder::readIdfMetadata(std::string &stream)
    {
        std::istringstream is(stream);
        kaitai::kstream ks(&is);
        column_data_idfmeta_t idf_meta(&ks);
        return IdfMetadata{
            idf_meta.blocks()->cp()->cs()->ss()->min_data_id(),
            idf_meta.blocks()->cp()->cs()->cs()->count_bit_packed(),
            idf_meta.bit_width()};
    }

    VertipaqDecoder::VertipaqDecoder(ClientContext &context, const std::string &path, const bool error_code)
    {
        this->error_code = error_code;
        auto &fs = FileSystem::GetFileSystem(context);
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

        // auto datamodel_ofs = 0;
        // auto datamodel_size = file_handle->GetFileSize();
        file_handle->Seek(datamodel_ofs);
        XPress9Wrapper xpress9_wrapper;
        if (!xpress9_wrapper.Initialize())
        {
            throw std::runtime_error("Failed to initialize XPress9Wrapper");
        }

        // Seek to the start of the DataModel compressed data
        std::vector<uint8_t> signature(ABF_XPRESS9_SIGNATURE);
        file_handle->Read(reinterpret_cast<char *>(signature.data()), ABF_XPRESS9_SIGNATURE);

        bytes_read += ABF_XPRESS9_SIGNATURE;

        while (bytes_read < datamodel_size)
        {
            uint32_t uncompressed_size;
            uint32_t compressed_size;
            // Read the compressed and uncompressed sizes before the offset
            file_handle->Read(reinterpret_cast<char *>(&uncompressed_size), sizeof(uint32_t));
            file_handle->Read(reinterpret_cast<char *>(&compressed_size), sizeof(uint32_t));
            bytes_read += sizeof(uint32_t) + sizeof(uint32_t);

            // Allocate buffers for compressed and decompressed data
            std::vector<uint8_t> decompressed_buffer(uncompressed_size);
            std::vector<uint8_t> compressed_buffer(compressed_size);

            file_handle->Read(reinterpret_cast<char *>(compressed_buffer.data()), compressed_size);
            bytes_read += compressed_size;

            // Decompress the entire data
            uint32_t decompressed_size = xpress9_wrapper.Decompress(compressed_buffer.data(), compressed_size, decompressed_buffer.data(), decompressed_buffer.size());
            // Verify that the total decompressed size matches the expected size
            if (decompressed_size != uncompressed_size)
            {
                throw std::runtime_error("Mismatch in decompressed block size in first block.");
            }
            all_decompressed_data.insert(all_decompressed_data.end(), decompressed_buffer.begin(), decompressed_buffer.end());
        }
    }

    void VertipaqDecoder::processVertipaqData(VertipaqDetails &details, VertipaqFiles &vfiles, duckdb::DataChunk &output, idx_t &out_idx, idx_t col_idx) {
        // Common processing
        std::string idf_metadata = details.IDF + "meta";
        auto &meta_file = vfiles[idf_metadata];
        std::string idf_meta_stream(all_decompressed_data.begin() + meta_file.m_cbOffsetHeader, 
                                    all_decompressed_data.begin() + meta_file.m_cbOffsetHeader + meta_file.Size);
        IdfMetadata idf_m = readIdfMetadata(idf_meta_stream);

        // Read IDF with error correction
        int correction = error_code ? 4 : 0;
        auto &idf_file = vfiles[details.IDF];
        std::string idf_stream(all_decompressed_data.begin() + idf_file.m_cbOffsetHeader, 
                            all_decompressed_data.begin() + idf_file.m_cbOffsetHeader + idf_file.Size - correction);

        //if isNullable treat min_data_id as blank
        int null_adjustment = details.IsNullable && details.DataType==2 ? 1 : 0;

        auto decoded_indices = readRLEBitPackedHybrid(idf_stream, idf_m.count_bit_packed, idf_m.min_data_id - null_adjustment, idf_m.bit_width);

        if (details.Dictionary != "")
        {
            // read dictionary file
            std::string dictionary_stream(all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader + vfiles[details.Dictionary].Size);
            auto dictionary = readDictionary(dictionary_stream, idf_m.min_data_id);

            if (details.DataType == 10) // FLOAT or DOUBLE
            {
                for (idx_t i = 0; i < std::min<int>(decoded_indices.size(), STANDARD_VECTOR_SIZE); i++)
                {
                    auto val = dictionary[decoded_indices[i]];
                    output.SetValue(col_idx, i, duckdb::Value(std::stoi(val) / 10000.0000));
                    out_idx++;
                }
            }
            else if (details.DataType == 9) // DATE
            {
                for (idx_t i = 0; i < std::min<int>(decoded_indices.size(), STANDARD_VECTOR_SIZE); i++)
                {
                    auto val = dictionary[decoded_indices[i]];
                    auto dd = date_t(0) + static_cast<int>(std::stod(val)) - EPOCH_ADJUSTMENT - idf_m.min_data_id; // days between (1900 - 1970)
                    output.SetValue(col_idx, i, duckdb::Value::DATE(dd));
                    out_idx++;
                }
            }
            else
            {
                for (idx_t i = 0; i < std::min<int>(decoded_indices.size(), STANDARD_VECTOR_SIZE); i++)
                {
                    auto val = dictionary[decoded_indices[i]];
                    output.SetValue(col_idx, i, val);
                    out_idx++;
                }
            }
        } else {
            if(details.DataType == 10) //DOUBLE or FLOAT
            {
                for (idx_t i = 0; i < std::min<int>(decoded_indices.size(), STANDARD_VECTOR_SIZE); i++)
                {
                    auto val = (decoded_indices[i] + details.BaseId) / details.Magnitude / 10000.0000;
                    output.SetValue(col_idx, i, val);
                    out_idx++;
                }
            } else {
                for (idx_t i = 0; i < std::min<int>(decoded_indices.size(), STANDARD_VECTOR_SIZE); i++)
                {
                    auto val = (decoded_indices[i] + details.BaseId) / details.Magnitude;
                    output.SetValue(col_idx, i, duckdb::Value::BIGINT(val));
                    out_idx++;
                }
            }
        }
    }

} // namespace duckdb
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

            // Assuming there's only one dictionary page for simplification
            auto stringData = static_cast<column_data_dictionary_t::string_data_t *>(dictionary.data());
            auto page = stringData->dictionary_pages();
            if (page->page_compressed())
            {
                auto compressed_store = static_cast<column_data_dictionary_t::compressed_strings_t *>(page->string_store());
                auto encode_array = compressed_store->encode_array();

                std::vector<uint8_t> target_vector(encode_array->begin(), encode_array->end());
                auto ui_decode_bits =  compressed_store->ui_decode_bits();
                auto compressed_string_buffer = compressed_store->compressed_string_buffer();
                
                // Huffman decode table
                HuffmanTable decodeTable;
                createDecodeTables(target_vector, decodeTable, ui_decode_bits, ui_decode_bits);

                // Assuming you have a status structure for error handling
                // XPRESS9_STATUS status = {0}; // Make sure the status is initialized correctly
                // HUFFMAN_DECODE_TABLE_ENTRY decodeTable[65536]; // Example size, adjust as needed
                // HUFFMAN_DECODE_NODE scratch[256 * 2]; // Temporary scratch space for node storage

                // // Fill decoding tables
                // uint32_t decodeTableSize = sizeof(decodeTable) / sizeof(decodeTable[0]);
                // HUFFMAN_DECODE_TABLE_ENTRY* tableEntriesUsedPtr = HuffmanCreateDecodeTables(
                //     &status,
                //     encode_array,
                //     256, // Alphabet size
                //     decodeTable,
                //     decodeTableSize,
                //     ui_decode_bits, // Root bits determined by your uiDecodeBits
                //     ui_decode_bits, // Tail bits could be the same or different
                //     scratch
                // );
                // Now you can decode using decodeTable and CompressedStringBuffer

                //cout the decoded table
                for (int i = 0; i < 256; i++)
                {
                    std::cout << "Decode table: " << i << " " << decodeTable[i] << std::endl;
                }
                
                throw std::runtime_error("Compressed string dictionary not supported");
            } else {
                auto uncompressed_store = static_cast<column_data_dictionary_t::uncompressed_strings_t *>(page->string_store());
                std::string uncompressed = uncompressed_store->uncompressed_character_buffer();
                // Extracting strings and filling the hashtable
                std::istringstream ss(uncompressed);
                std::string token;
                int index = min_data_id;
                while (std::getline(ss, token, '\0'))
                { // assuming null-terminated strings in buffer
                    hashtable[index++] = token;
                }
                return hashtable;
            }
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

    std::vector<std::string> VertipaqDecoder::processVertipaqStr(VertipaqDetails &details, VertipaqFiles &vfiles)
    {

        if (details.Dictionary.empty())
        {
            throw std::runtime_error("Dictionary is empty");
        }
        // append 'meta' to IDF filename
        std::string idf_metadata = details.IDF + "meta";
        std::string idf_meta_stream(all_decompressed_data.begin() + vfiles[idf_metadata].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[idf_metadata].m_cbOffsetHeader + vfiles[idf_metadata].Size);
        std::istringstream is(idf_meta_stream);
        IdfMetadata idf_m = readIdfMetadata(idf_meta_stream);
        // read dictionary file
        std::string dictionary_stream(all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader + vfiles[details.Dictionary].Size);
        auto dictionary = readDictionary(dictionary_stream, idf_m.min_data_id);
        // read IDF
        auto correction = error_code ? 4 : 0;
        std::string idf_stream(all_decompressed_data.begin() + vfiles[details.IDF].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[details.IDF].m_cbOffsetHeader + vfiles[details.IDF].Size - correction);

        int null_adjustment = details.IsNullable && details.DataType==2 ? 1 : 0;

        auto vector = readRLEBitPackedHybrid(idf_stream, idf_m.count_bit_packed, idf_m.min_data_id - null_adjustment, idf_m.bit_width);

        std::vector<std::string> output;
        for (int i = 0; i < vector.size(); i++)
        {
            output.push_back(dictionary[vector[i]]);
        }
        return output;
    }

    std::vector<uint64_t> VertipaqDecoder::processVertipaqInt(VertipaqDetails &details, VertipaqFiles &vfiles)
    {

        // append 'meta' to IDF filename
        std::string idf_metadata = details.IDF + "meta";
        std::string idf_meta_stream(all_decompressed_data.begin() + vfiles[idf_metadata].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[idf_metadata].m_cbOffsetHeader + vfiles[idf_metadata].Size);
        std::istringstream is(idf_meta_stream);
        IdfMetadata idf_m = readIdfMetadata(idf_meta_stream);
        std::map<uint64_t, std::string> dictionary;

        // read IDF
        auto correction = error_code ? 4 : 0;
        std::string idf_stream(all_decompressed_data.begin() + vfiles[details.IDF].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[details.IDF].m_cbOffsetHeader + vfiles[details.IDF].Size - correction);

        auto vector = readRLEBitPackedHybrid(idf_stream, idf_m.count_bit_packed, idf_m.min_data_id, idf_m.bit_width);

        std::vector<uint64_t> output;
        for (int i = 0; i < vector.size(); i++)
        {
            output.push_back((vector[i] + details.BaseId) / details.Magnitude);
        }

        return output;
    }
/*
    void VertipaqDecoder::processVertipaqData(VertipaqDetails &details, VertipaqFiles &vfiles, duckdb::Vector &output) {
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

        // Decode data
        auto decoded_indices = readRLEBitPackedHybrid(idf_stream, idf_m.count_bit_packed, idf_m.min_data_id, idf_m.bit_width);

        // Type-specific processing directly into DuckDB vector
        // if (details.DataType == STRING_TYPE) {
        if (details.DataType == 2) {
            std::string dictionary_stream(all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader,
                                        all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader + vfiles[details.Dictionary].Size);
            auto dictionary = readDictionary(dictionary_stream, idf_m.min_data_id);
            // duckdb::FlatVector::SetData(output, (duckdb::data_ptr_t)dictionary.data()); // Example setting vector data
        } else {
            // Assuming integer processing
            std::vector<int64_t> processed_values;
            for (auto idx : decoded_indices) {
                processed_values.push_back((idx + details.BaseId) / details.Magnitude);
            }
            duckdb::FlatVector::SetData(output, (duckdb::data_ptr_t)processed_values.data());
        }
    }
*/
} // namespace duckdb
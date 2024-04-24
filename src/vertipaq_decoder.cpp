#include "vertipaq_decoder.hpp"
#include <iostream>
namespace duckdb
{
    std::vector<int> VertipaqDecoder::readBitPacked(const std::vector<uint64_t> &sub_segment, int bit_width, int min_data_id)
    {
        int mask = (1 << bit_width) - 1;
        std::vector<int> res;
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

    std::vector<std::string> VertipaqDecoder::extractStrings(std::istream &stream)
    {
        std::vector<std::string> strings;
        std::string temp;
        while (std::getline(stream, temp, '\0'))
        {
            if (!temp.empty())
                strings.push_back(temp);
        }
        return strings;
    }

    // std::vector<int> VertipaqDecoder::readRLEBitPackedHybrid(kaitai::kstream ks, int entries, int min_data_id, int bit_width)
    // {

    //     return vector;
    // }

    void VertipaqDecoder::processVertipaqData(ClientContext &context, const std::string &path, VertipaqDetails &details, VertipaqFiles &vfiles, const bool error_code)
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

        // Seek to the start of the DataModel compressed data
        std::vector<uint8_t> signature(ABF_XPRESS9_SIGNATURE);
        file_handle->Read(reinterpret_cast<char *>(signature.data()), ABF_XPRESS9_SIGNATURE);

        bytes_read += ABF_XPRESS9_SIGNATURE;

        std::vector<uint8_t> all_decompressed_data;
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
        // append 'meta' to IDF
        std::string idf_metadata = details.IDF + "meta";
        // read from the decompressed with offset and size from details
        std::string idf_meta_stream(all_decompressed_data.begin() + vfiles[idf_metadata].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[idf_metadata].m_cbOffsetHeader + vfiles[idf_metadata].Size);

        std::istringstream is(idf_meta_stream);
        kaitai::kstream ks(&is);
        column_data_idfmeta_t idf_meta(&ks);
        IdfMetadata idf_m = {
            idf_meta.blocks()->cp()->cs()->ss()->min_data_id(),
            idf_meta.blocks()->cp()->cs()->cs()->count_bit_packed(),
            idf_meta.bit_width()};
        ks.close();

        // if Dictionary is not empty, read the dictionary
        if (!details.Dictionary.empty())
        {
            std::string dictionary_stream(all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[details.Dictionary].m_cbOffsetHeader + vfiles[details.Dictionary].Size);
            // auto dictionary = readDictionary(dictionary_stream, details.BaseId);
            std::istringstream is_d(dictionary_stream);
            kaitai::kstream ks_d(&is_d);
            column_data_dictionary_t dictionary(&ks_d);

            // Checking dictionary type and processing accordingly
            if (dictionary.dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_STRING)
            {
                // Handling string-based dictionary
                std::unordered_map<int, std::string> hashtable;

                // Assuming there's only one dictionary page for simplification
                auto stringData = static_cast<column_data_dictionary_t::string_data_t *>(dictionary.data());
                auto page = stringData->dictionary_pages();
                std::string uncompressed = page->string_store()->uncompressed_character_buffer();

                // Extracting strings and filling the hashtable
                std::istringstream ss(uncompressed);
                std::string token;
                int index = idf_m.min_data_id;
                while (std::getline(ss, token, '\0'))
                { // assuming null-terminated strings in buffer
                    hashtable[index++] = token;
                }

                std::cout << "STR  Dictionary size: " << hashtable.size() << std::endl;
                // return hashtable;
            }
            else if (dictionary.dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_LONG ||
                     dictionary.dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_REAL)
            {
                // Handling numeric data
                std::unordered_map<int, double> values;
                auto numberData = static_cast<column_data_dictionary_t::number_data_t *>(dictionary.data());
                auto vector_info = numberData->vector_of_vectors_info();
                const auto &vals = vector_info->values();
                int index = idf_m.min_data_id;
                for (double val : *vals)
                {
                    values[index++] = val;
                }

                // Converting to string map for a unified interface (if needed)
                std::unordered_map<int, std::string> str_values;
                for (const auto &pair : values)
                {
                    str_values[pair.first] = std::to_string(pair.second);
                }

                std::cout << "REAL Dictionary size: " << str_values.size() << std::endl;
                // return str_values;
            }
            // ks_d.close();
        }
        // read IDF
        auto  correction =  error_code ? 4: 0;
        std::string idf_stream(all_decompressed_data.begin() + vfiles[details.IDF].m_cbOffsetHeader, all_decompressed_data.begin() + vfiles[details.IDF].m_cbOffsetHeader + vfiles[details.IDF].Size-correction);
        std::istringstream stream(idf_stream);
        kaitai::kstream ks_idf(&stream);
        // auto result = readRLEBitPackedHybrid(ks_idf, idf_m.count_bit_packed, idf_m.min_data_id, idf_m.bit_width);
        column_data_idf_t column_data(&ks_idf);

        std::vector<int> bitpacked_values;
        std::vector<int> vector;
        int bit_packed_entries = 0;
        int bit_packed_offset = 0;

        if (idf_m.count_bit_packed > 0 && !column_data.segments()->empty())
        {
            auto &segments = *column_data.segments();
            auto segment = segments[0]; // Assuming there is at least one segment
            int size = segment->sub_segment_size();
            if (segment->sub_segment()->back() == 0 && size == 1)
            {
                bitpacked_values.assign(idf_m.count_bit_packed, idf_m.min_data_id);
            }
            else
            {
                // Assuming `sub_segment` is accessible and contains meaningful data
                const auto &sub_segment = *segment->sub_segment();
                bitpacked_values = readBitPacked(sub_segment, idf_m.bit_width, idf_m.min_data_id);
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
                    std::vector<int> rle(entry->repeat_value(), entry->data_value());
                    vector.insert(vector.end(), rle.begin(), rle.end());
                }
            }
        }
        // ks_idf.close();
        // print all of the values
        for (int i = 0; i < vector.size(); i++)
        {
            std::cout << vector[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "IDF size: " << vector.size() << std::endl;
    }
} // namespace duckdb
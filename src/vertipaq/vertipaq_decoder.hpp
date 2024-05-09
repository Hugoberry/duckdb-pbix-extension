#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include "vertipaq_meta.hpp"
#include "Xpress9Wrapper.h"
#include "zip_utils.h"
#include "kaitaistream.h"
#include "column_data_idf.h"
#include "column_data_idfmeta.h"
#include "column_data_dictionary.h"

#include "duckdb/main/client_context.hpp"
namespace duckdb
{
    constexpr uint32_t EPOCH_ADJUSTMENT = 25566; //days between (1900 - 1970)
    class VertipaqDecoder
    {
    
    private:

        std::vector<uint8_t> all_decompressed_data;
        bool error_code;

    public:
        VertipaqDecoder(ClientContext &context, const std::string &path, const bool error_code);
        std::vector<uint64_t> readBitPacked(const std::vector<uint64_t> &sub_segment, uint64_t bit_width, uint64_t min_data_id);
        std::vector<uint64_t> readRLEBitPackedHybrid(std::string &str, uint64_t entries, uint64_t min_data_id, uint64_t bit_width);
        std::map<uint64_t, std::string> readDictionary(std::string &dictionary_stream, uint64_t min_data_id);
        IdfMetadata readIdfMetadata(std::string &stream);
        void processVertipaqData(VertipaqDetails &details, VertipaqFiles &vfiles, duckdb::DataChunk &output, idx_t &out_idx, idx_t col_idx);
    };
} // namespace duckdb
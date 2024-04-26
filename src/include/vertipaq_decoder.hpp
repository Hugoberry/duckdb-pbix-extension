#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include "vertipaq_meta.hpp"
#include "Xpress9Wrapper.h"
#include "zip_archive.hpp"
#include "kaitaistream.h"
#include "column_data_idf.h"
#include "column_data_idfmeta.h"
#include "column_data_dictionary.h"

#include "duckdb/main/client_context.hpp"
namespace duckdb
{
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
        std::vector<std::string> processVertipaqStr(VertipaqDetails &details, VertipaqFiles &vfiles);
        std::vector<uint64_t> processVertipaqInt(VertipaqDetails &details, VertipaqFiles &vfiles);
    };
} // namespace duckdb
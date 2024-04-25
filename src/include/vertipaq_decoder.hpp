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
    class VertipaqDecoder
    {
    public:
        static std::vector<uint64_t> readBitPacked(const std::vector<uint64_t>& sub_segment, uint64_t bit_width, uint64_t min_data_id);
        static std::vector<uint64_t> readRLEBitPackedHybrid(std::string& str, uint64_t entries, uint64_t min_data_id, uint64_t bit_width);
        static std::map<uint64_t, std::string> readDictionary(std::string& dictionary_stream, uint64_t min_data_id);
        static IdfMetadata readIdfMetadata(std::string& stream);

        static void processVertipaqData(ClientContext &context, const std::string &path, VertipaqDetails &details, VertipaqFiles &vfiles, const bool error_code);
    };
} // namespace duckdb
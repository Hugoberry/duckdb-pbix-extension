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
        static std::vector<int> readBitPacked(const std::vector<uint64_t>& sub_segment, int bit_width, int min_data_id);
        static std::vector<std::string> extractStrings(std::istream &stream);
        static std::vector<int> readRLEBitPackedHybrid(std::istringstream &stream, int entries, int min_data_id, int bit_width);
        static std::map<int, std::string> readDictionary(std::istream &stream, int minDataId);

        static void processVertipaqData(ClientContext &context, const std::string &path, VertipaqDetails &details, VertipaqFiles &vfiles);
    };
} // namespace duckdb
#pragma once
#include <string>
#include "duckdb.hpp"
struct VertipaqFile
{
	std::string Path;
	std::string FileName;
	std::string StoragePath;
	uint64_t Size;
	uint64_t m_cbOffsetHeader;
};


struct VertipaqDetails
{
	int64_t StoragePosition;
	std::string Dictionary;
	std::string IDF;
	int64_t DataType;
	int64_t BaseId;
	double Magnitude;
};

struct IdfMetadata
{
	uint32_t min_data_id;
	uint64_t count_bit_packed;
	int32_t bit_width;
};

struct DataModel{
	std::vector<uint8_t> metadata_db; 
	std::vector<VertipaqFile> vertipaq_files;
	bool error_code;
};

using VertipaqFiles = std::map<std::string, VertipaqFile>;
using VertipaqData = std::map<std::string, VertipaqDetails>;

// Constants related to ZIP file parsing
constexpr unsigned char ZIP_LOCAL_FILE_HEADER_FIXED = 26;
constexpr unsigned char ZIP_LOCAL_FILE_HEADER = 30;
constexpr unsigned char ABF_XPRESS9_SIGNATURE = 102;
constexpr unsigned char ABF_BACKUP_LOG_HEADER_OFFSET = 72;
constexpr uint32_t BLOCK_SIZE = 0x200000;
constexpr unsigned short ABF_BACKUP_LOG_HEADER_SIZE = 0x1000 - ABF_BACKUP_LOG_HEADER_OFFSET;
static constexpr duckdb::idx_t FILE_READ = duckdb::idx_t(1 << 0);
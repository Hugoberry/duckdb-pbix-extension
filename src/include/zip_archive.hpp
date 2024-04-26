#pragma once
#include "duckdb/common/typedefs.hpp"
#include "duckdb/common/serializer/buffered_file_reader.hpp"
#include "miniz.hpp"
#include "vertipaq_meta.hpp"

namespace duckdb {

class ZipArchiveExtractStream;

class ZipArchiveFileHandle : public std::enable_shared_from_this<ZipArchiveFileHandle> {
private:
	FileHandle *file_handle;
	duckdb_miniz::mz_zip_archive zip_archive;

public:
	// Dont ever call this constructor directly, use ZipArchiveFileHandle::Open instead
	explicit ZipArchiveFileHandle(unique_ptr<FileHandle> file_handle_p);
	static shared_ptr<ZipArchiveFileHandle> Open(FileSystem &fs, const string &file_name);
	unique_ptr<ZipArchiveExtractStream> Extract(const string &item_name);
	unique_ptr<ZipArchiveExtractStream> Extract(int item_index);
	vector<pair<string, int>> GetEntries();
	vector<pair<string, int>> SearchEntries(std::function<bool(const string &)> &&predicate);
	bool TryGetEntryIndexByName(const string &name, int &index);
	~ZipArchiveFileHandle();
};

class ZipArchiveExtractStream {
private:
	// The shared pointer ensures that the archive handle stays alive as long as the stream is alive
	shared_ptr<ZipArchiveFileHandle> archive_handle;
	duckdb_miniz::mz_zip_reader_extract_iter_state *iter_state;
	idx_t bytes_read;
	idx_t bytes_total;

public:
	ZipArchiveExtractStream(shared_ptr<ZipArchiveFileHandle> archive_handle_p,
	                        duckdb_miniz::mz_zip_reader_extract_iter_state *iter_state_p, idx_t bytes_total_p);
	idx_t Read(void *buffer, idx_t buffer_size);
	bool IsDone();
	~ZipArchiveExtractStream();
};

} // namespace duckdb
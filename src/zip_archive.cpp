#include "duckdb/common/typedefs.hpp"
#include "duckdb/common/serializer/buffered_file_reader.hpp"
#include "duckdb/common/pair.hpp"

#include "zip_archive.hpp"

namespace duckdb {

// TODO: Remove this and port into to minizip-ng
using namespace duckdb_miniz;

//------------------------------------------------------------------------------
// Archive Handle
//------------------------------------------------------------------------------

ZipArchiveFileHandle::ZipArchiveFileHandle(unique_ptr<FileHandle> file_handle_p) {

	// zero out zip archive memory
	mz_zip_zero_struct(&zip_archive);

	// Pass userdata opaque pointer to file handle
	zip_archive.m_pIO_opaque = file_handle_p.get();

	// Setup read callback
	zip_archive.m_pRead = [](void *user_data, mz_uint64 file_offset, void *buffer, size_t n) {
		auto handle = static_cast<FileHandle *>(user_data);
		handle->Seek(file_offset);
		auto bytes = handle->Read(buffer, n);
		return static_cast<size_t>(bytes);
	};

	zip_archive.m_pWrite = [](void *user_data, mz_uint64 file_offset, const void *buffer, size_t n) {
		auto handle = static_cast<FileHandle *>(user_data);
		handle->Seek(file_offset);
		auto bytes = handle->Write(const_cast<void *>(buffer), n);
		return static_cast<size_t>(bytes);
	};

	// Initialize archive
	if (!mz_zip_reader_init(&zip_archive, file_handle_p->GetFileSize(), MZ_ZIP_FLAG_COMPRESSED_DATA)) {
		auto error = mz_zip_get_last_error(&zip_archive);
		auto error_str = mz_zip_get_error_string(error);
		throw IOException("Failed to initialize zip archive: %s", error_str);
	}

	// Set file handle down here so that it is only set released if the archive is successfully initialized
	file_handle = file_handle_p.release();
}

shared_ptr<ZipArchiveFileHandle> ZipArchiveFileHandle::Open(FileSystem &fs, const string &file_name) {
	auto file_handle = fs.OpenFile(file_name, FILE_READ);
	if (!file_handle) {
		return nullptr;
	}
	return make_shared<ZipArchiveFileHandle>(std::move(file_handle));
}

unique_ptr<ZipArchiveExtractStream> ZipArchiveFileHandle::Extract(const string &item_name) {
	auto item_index = mz_zip_reader_locate_file(&zip_archive, item_name.c_str(), nullptr, 0);
	if (item_index < 0) {
		return nullptr;
	}
	return Extract(item_index);
}

unique_ptr<ZipArchiveExtractStream> ZipArchiveFileHandle::Extract(int item_index) {
	auto iter_state = mz_zip_reader_extract_iter_new(&zip_archive, item_index, 0);
	if (!iter_state) {
		throw IOException("Failed to initialize extract iterator for item %d", item_index);
	}

	mz_zip_archive_file_stat file_stat;
	auto ok = mz_zip_reader_file_stat(&zip_archive, item_index, &file_stat);
	if (!ok) {
		throw IOException("Failed to read file stat for item %d", item_index);
	}

	return make_uniq<ZipArchiveExtractStream>(shared_from_this(), iter_state, file_stat.m_uncomp_size);
}

vector<pair<string, int>> ZipArchiveFileHandle::GetEntries() {
	vector<pair<string, int>> result;
	for (int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {
		mz_zip_archive_file_stat file_stat;
		auto ok = mz_zip_reader_file_stat(&zip_archive, i, &file_stat);
		if (!ok) {
			throw IOException("Failed to read file stat for item %d", i);
		}
		result.emplace_back(file_stat.m_filename, i);
	}
	return result;
}

vector<pair<string, int>> ZipArchiveFileHandle::SearchEntries(std::function<bool(const string &)> &&predicate) {
	vector<pair<string, int>> result;
	for (int i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++) {
		mz_zip_archive_file_stat file_stat;
		auto ok = mz_zip_reader_file_stat(&zip_archive, i, &file_stat);
		if (!ok) {
			throw IOException("Failed to read file stat for item %d", i);
		}
		auto str = string(file_stat.m_filename);
		if (predicate(str)) {
			result.emplace_back(str, i);
		}
	}
	return result;
}

bool ZipArchiveFileHandle::TryGetEntryIndexByName(const string &name, int &index) {
	index = mz_zip_reader_locate_file(&zip_archive, name.c_str(), nullptr, 0);
	return index >= 0;
}

ZipArchiveFileHandle::~ZipArchiveFileHandle() {
	mz_zip_reader_end(&zip_archive);
	delete file_handle;
}

//------------------------------------------------------------------------------
// Extract stream
//------------------------------------------------------------------------------

ZipArchiveExtractStream::ZipArchiveExtractStream(shared_ptr<ZipArchiveFileHandle> archive_handle_p,
                                                 duckdb_miniz::mz_zip_reader_extract_iter_state *iter_state_p,
                                                 idx_t bytes_total_p)
    : archive_handle(std::move(archive_handle_p)), iter_state(iter_state_p), bytes_read(0), bytes_total(bytes_total_p) {
}

idx_t ZipArchiveExtractStream::Read(void *buffer, idx_t buffer_size) {
	auto bytes = duckdb_miniz::mz_zip_reader_extract_iter_read(iter_state, buffer, buffer_size);
	bytes_read += bytes;
	return bytes;
}

bool ZipArchiveExtractStream::IsDone() {
	return bytes_read == bytes_total;
}

ZipArchiveExtractStream::~ZipArchiveExtractStream() {
	mz_zip_reader_extract_iter_free(iter_state);
}

} // namespace duckdb
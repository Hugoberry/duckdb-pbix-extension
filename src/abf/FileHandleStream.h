#pragma once

#include <istream>
#include <streambuf>
#include <vector>
#include "duckdb/common/file_system.hpp"

namespace duckdb {

class FileHandleStreamBuf : public std::streambuf {
public:
    explicit FileHandleStreamBuf(FileHandle* handle);

protected:
    // Override underflow() to handle reading from the custom file system
    int underflow() override;

    // Override overflow() if you need write capability
    int overflow(int c = traits_type::eof()) override;

    // Override seekoff to handle seeking
    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir dir,
                           std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override;

private:
    FileHandle* handle;
    std::vector<char> buffer;
};

class FileHandleStream : public std::istream {
public:
    explicit FileHandleStream(FileHandle* handle);

private:
    FileHandleStreamBuf buf;
};

} // namespace duckdb

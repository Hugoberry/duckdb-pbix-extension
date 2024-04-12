#include <iostream>
#include <streambuf>
#include "duckdb/common/file_system.hpp"

class FileHandleStreamBuf : public std::streambuf {
public:
    FileHandleStreamBuf(duckdb::FileHandle* handle) : handle(handle) {}

protected:
    // Override underflow() to handle reading from the custom file system
    int underflow() override {
        if (gptr() < egptr()) {
            return traits_type::to_int_type(*gptr());
        }
        char* base = &buffer[0];
        setg(base, base, base);
        std::size_t n = handle->Read(base, buffer.size());
        if (n == 0) return traits_type::eof();
        setg(base, base, base + n);
        return traits_type::to_int_type(*gptr());
    }

    // Override overflow() if you need write capability
    int overflow(int c = traits_type::eof()) override {
        return traits_type::eof(); // Indicate always full buffer (no writing)
    }

    // Override seekoff to handle seeking
    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir dir,
                           std::ios_base::openmode which = std::ios_base::in | std::ios_base::out) override {
        if (dir == std::ios_base::cur) {
            off += handle->SeekPosition();
        } else if (dir == std::ios_base::end) {
            off += handle->GetFileSize();
        }
        handle->Seek(off);
        return handle->SeekPosition();
    }

private:
    duckdb::FileHandle* handle;
    std::vector<char> buffer = std::vector<char>(1024);
};

class FileHandleStream : public std::istream {
public:
    FileHandleStream(duckdb::FileHandle* handle)
        : std::istream(nullptr), buf(handle) {
        rdbuf(&buf);
    }

private:
    FileHandleStreamBuf buf;
};

#ifndef UNZIP_DATAMODEL_H_
#define UNZIP_DATAMODEL_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <vector>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

class unzip_datamodel_t : public kaitai::kstruct {

public:
    class end_of_central_dir_t;
    class central_dir_entry_t;
    class local_file_header_t;

    unzip_datamodel_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, unzip_datamodel_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~unzip_datamodel_t();

    class end_of_central_dir_t : public kaitai::kstruct {

    public:

        end_of_central_dir_t(kaitai::kstream* p__io, unzip_datamodel_t* p__parent = 0, unzip_datamodel_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~end_of_central_dir_t();

    private:
        std::string m_eocd_magic;
        uint16_t m_disk_of_end_of_central_dir;
        uint16_t m_disk_of_central_dir;
        uint16_t m_num_central_dir_entries_on_disk;
        uint16_t m_num_central_dir_entries_total;
        uint32_t m_len_central_dir;
        uint32_t m_ofs_central_dir;
        uint16_t m_len_comment;
        unzip_datamodel_t* m__root;
        unzip_datamodel_t* m__parent;

    public:
        std::string eocd_magic() const { return m_eocd_magic; }
        uint16_t disk_of_end_of_central_dir() const { return m_disk_of_end_of_central_dir; }
        uint16_t disk_of_central_dir() const { return m_disk_of_central_dir; }
        uint16_t num_central_dir_entries_on_disk() const { return m_num_central_dir_entries_on_disk; }
        uint16_t num_central_dir_entries_total() const { return m_num_central_dir_entries_total; }
        uint32_t len_central_dir() const { return m_len_central_dir; }
        uint32_t ofs_central_dir() const { return m_ofs_central_dir; }
        uint16_t len_comment() const { return m_len_comment; }
        unzip_datamodel_t* _root() const { return m__root; }
        unzip_datamodel_t* _parent() const { return m__parent; }
    };

    class central_dir_entry_t : public kaitai::kstruct {

    public:

        central_dir_entry_t(kaitai::kstream* p__io, unzip_datamodel_t* p__parent = 0, unzip_datamodel_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~central_dir_entry_t();

    private:
        bool f_local_header;
        local_file_header_t* m_local_header;

    public:
        local_file_header_t* local_header();

    private:
        std::string m_cd_magic;
        uint16_t m_version_made_by;
        uint16_t m_version_needed_to_extract;
        uint16_t m_flags;
        uint16_t m_compression_method;
        std::string m_file_mod_time;
        uint32_t m_crc32;
        uint32_t m_len_body_compressed;
        uint32_t m_len_body_uncompressed;
        uint16_t m_len_file_name;
        uint16_t m_len_extra;
        uint16_t m_len_comment;
        uint16_t m_disk_number_start;
        uint16_t m_int_file_attr;
        uint32_t m_ext_file_attr;
        int32_t m_ofs_local_header;
        std::string m_file_name;
        std::string m_extra;
        std::string m_comment;
        unzip_datamodel_t* m__root;
        unzip_datamodel_t* m__parent;

    public:
        std::string cd_magic() const { return m_cd_magic; }
        uint16_t version_made_by() const { return m_version_made_by; }
        uint16_t version_needed_to_extract() const { return m_version_needed_to_extract; }
        uint16_t flags() const { return m_flags; }
        uint16_t compression_method() const { return m_compression_method; }
        std::string file_mod_time() const { return m_file_mod_time; }
        uint32_t crc32() const { return m_crc32; }
        uint32_t len_body_compressed() const { return m_len_body_compressed; }
        uint32_t len_body_uncompressed() const { return m_len_body_uncompressed; }
        uint16_t len_file_name() const { return m_len_file_name; }
        uint16_t len_extra() const { return m_len_extra; }
        uint16_t len_comment() const { return m_len_comment; }
        uint16_t disk_number_start() const { return m_disk_number_start; }
        uint16_t int_file_attr() const { return m_int_file_attr; }
        uint32_t ext_file_attr() const { return m_ext_file_attr; }
        int32_t ofs_local_header() const { return m_ofs_local_header; }
        std::string file_name() const { return m_file_name; }
        std::string extra() const { return m_extra; }
        std::string comment() const { return m_comment; }
        unzip_datamodel_t* _root() const { return m__root; }
        unzip_datamodel_t* _parent() const { return m__parent; }
    };

    class local_file_header_t : public kaitai::kstruct {

    public:

        local_file_header_t(kaitai::kstream* p__io, unzip_datamodel_t::central_dir_entry_t* p__parent = 0, unzip_datamodel_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~local_file_header_t();

    private:
        std::string m_lfh_magic;
        uint16_t m_version;
        std::string m_flags;
        uint16_t m_compression_method;
        std::string m_file_mod_time;
        uint32_t m_crc32;
        uint32_t m_len_body_compressed;
        uint32_t m_len_body_uncompressed;
        uint16_t m_len_file_name;
        uint16_t m_len_extra;
        std::string m_file_name;
        std::string m_extra;
        unzip_datamodel_t* m__root;
        unzip_datamodel_t::central_dir_entry_t* m__parent;

    public:
        std::string lfh_magic() const { return m_lfh_magic; }
        uint16_t version() const { return m_version; }
        std::string flags() const { return m_flags; }
        uint16_t compression_method() const { return m_compression_method; }
        std::string file_mod_time() const { return m_file_mod_time; }
        uint32_t crc32() const { return m_crc32; }
        uint32_t len_body_compressed() const { return m_len_body_compressed; }
        uint32_t len_body_uncompressed() const { return m_len_body_uncompressed; }
        uint16_t len_file_name() const { return m_len_file_name; }
        uint16_t len_extra() const { return m_len_extra; }
        std::string file_name() const { return m_file_name; }
        std::string extra() const { return m_extra; }
        unzip_datamodel_t* _root() const { return m__root; }
        unzip_datamodel_t::central_dir_entry_t* _parent() const { return m__parent; }
    };

private:
    bool f_end_of_central_dir;
    end_of_central_dir_t* m_end_of_central_dir;

public:
    end_of_central_dir_t* end_of_central_dir();

private:
    bool f_central_dir;
    std::vector<central_dir_entry_t*>* m_central_dir;

public:
    std::vector<central_dir_entry_t*>* central_dir();

private:
    unzip_datamodel_t* m__root;
    kaitai::kstruct* m__parent;

public:
    unzip_datamodel_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // UNZIP_DATAMODEL_H_

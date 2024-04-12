#ifndef PBIX_H_
#define PBIX_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <vector>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

class pbix_t : public kaitai::kstruct {

public:
    class local_file_t;
    class data_descriptor_t;
    class central_dir_entry_t;
    class pk_section_t;
    class local_file_header_t;
    class end_of_central_dir_t;

    pbix_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, pbix_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~pbix_t();

    class local_file_t : public kaitai::kstruct {

    public:

        local_file_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent = 0, pbix_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~local_file_t();

    private:
        local_file_header_t* m_header;
        std::string m_body;
        pbix_t* m__root;
        pbix_t::pk_section_t* m__parent;

    public:
        local_file_header_t* header() const { return m_header; }
        std::string body() const { return m_body; }
        pbix_t* _root() const { return m__root; }
        pbix_t::pk_section_t* _parent() const { return m__parent; }
    };

    class data_descriptor_t : public kaitai::kstruct {

    public:

        data_descriptor_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent = 0, pbix_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~data_descriptor_t();

    private:
        std::string m_data_descriptor_obs;
        pbix_t* m__root;
        pbix_t::pk_section_t* m__parent;

    public:
        std::string data_descriptor_obs() const { return m_data_descriptor_obs; }
        pbix_t* _root() const { return m__root; }
        pbix_t::pk_section_t* _parent() const { return m__parent; }
    };

    class central_dir_entry_t : public kaitai::kstruct {

    public:

        central_dir_entry_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent = 0, pbix_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~central_dir_entry_t();

    private:
        std::string m_header_obs;
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
        pbix_t* m__root;
        pbix_t::pk_section_t* m__parent;

    public:
        std::string header_obs() const { return m_header_obs; }
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
        pbix_t* _root() const { return m__root; }
        pbix_t::pk_section_t* _parent() const { return m__parent; }
    };

    class pk_section_t : public kaitai::kstruct {

    public:

        pk_section_t(kaitai::kstream* p__io, pbix_t* p__parent = 0, pbix_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~pk_section_t();

    private:
        std::string m_magic;
        uint16_t m_section_type;
        kaitai::kstruct* m_body;
        bool n_body;

    public:
        bool _is_null_body() { body(); return n_body; };

    private:
        pbix_t* m__root;
        pbix_t* m__parent;

    public:
        std::string magic() const { return m_magic; }
        uint16_t section_type() const { return m_section_type; }
        kaitai::kstruct* body() const { return m_body; }
        pbix_t* _root() const { return m__root; }
        pbix_t* _parent() const { return m__parent; }
    };

    class local_file_header_t : public kaitai::kstruct {

    public:

        local_file_header_t(kaitai::kstream* p__io, pbix_t::local_file_t* p__parent = 0, pbix_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~local_file_header_t();

    private:
        std::string m_header_trimmed;
        uint32_t m_len_body_compressed;
        uint32_t m_len_body_uncompressed;
        uint16_t m_len_file_name;
        uint16_t m_len_extra;
        std::string m_file_name;
        std::string m_extra;
        pbix_t* m__root;
        pbix_t::local_file_t* m__parent;

    public:
        std::string header_trimmed() const { return m_header_trimmed; }
        uint32_t len_body_compressed() const { return m_len_body_compressed; }
        uint32_t len_body_uncompressed() const { return m_len_body_uncompressed; }
        uint16_t len_file_name() const { return m_len_file_name; }
        uint16_t len_extra() const { return m_len_extra; }
        std::string file_name() const { return m_file_name; }
        std::string extra() const { return m_extra; }
        pbix_t* _root() const { return m__root; }
        pbix_t::local_file_t* _parent() const { return m__parent; }
    };

    class end_of_central_dir_t : public kaitai::kstruct {

    public:

        end_of_central_dir_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent = 0, pbix_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~end_of_central_dir_t();

    private:
        std::string m_header_obs;
        uint16_t m_len_comment;
        std::string m_comment;
        pbix_t* m__root;
        pbix_t::pk_section_t* m__parent;

    public:
        std::string header_obs() const { return m_header_obs; }
        uint16_t len_comment() const { return m_len_comment; }
        std::string comment() const { return m_comment; }
        pbix_t* _root() const { return m__root; }
        pbix_t::pk_section_t* _parent() const { return m__parent; }
    };

private:
    std::vector<pk_section_t*>* m_sections;
    pbix_t* m__root;
    kaitai::kstruct* m__parent;

public:
    std::vector<pk_section_t*>* sections() const { return m_sections; }
    pbix_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // PBIX_H_

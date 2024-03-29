#ifndef IDFMETA_PARSER_H_
#define IDFMETA_PARSER_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

class idfmeta_parser_t : public kaitai::kstruct {

public:
    class c_p_element_t;
    class c_s1_element_t;
    class c_s0_element_t;
    class s_d_os_element_t;
    class s_s_element_t;
    class block_t;
    class c_s_d_os1_element_t;
    class c_s_d_os_element_t;

    idfmeta_parser_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, idfmeta_parser_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~idfmeta_parser_t();

    class c_p_element_t : public kaitai::kstruct {

    public:

        c_p_element_t(kaitai::kstream* p__io, idfmeta_parser_t::block_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~c_p_element_t();

    private:
        std::string m_cp_tag;
        uint64_t m_version_one;
        c_s0_element_t* m_cs;
        std::string m_cp_tag_end;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t::block_t* m__parent;

    public:
        std::string cp_tag() const { return m_cp_tag; }
        uint64_t version_one() const { return m_version_one; }
        c_s0_element_t* cs() const { return m_cs; }
        std::string cp_tag_end() const { return m_cp_tag_end; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t::block_t* _parent() const { return m__parent; }
    };

    class c_s1_element_t : public kaitai::kstruct {

    public:

        c_s1_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_s0_element_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~c_s1_element_t();

    private:
        std::string m_cs_tag;
        uint64_t m_count_bit_packed;
        std::string m_blob_with9_zeros;
        std::string m_cs_tag_end;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t::c_s0_element_t* m__parent;

    public:
        std::string cs_tag() const { return m_cs_tag; }
        uint64_t count_bit_packed() const { return m_count_bit_packed; }
        std::string blob_with9_zeros() const { return m_blob_with9_zeros; }
        std::string cs_tag_end() const { return m_cs_tag_end; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t::c_s0_element_t* _parent() const { return m__parent; }
    };

    class c_s0_element_t : public kaitai::kstruct {

    public:

        c_s0_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_p_element_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~c_s0_element_t();

    private:
        std::string m_cs_tag;
        uint64_t m_records;
        uint64_t m_one;
        uint32_t m_a_b_a_5_a;
        uint32_t m_iterator;
        uint64_t m_bookmark_bits_1_2_8;
        uint64_t m_storage_alloc_size;
        uint64_t m_storage_used_size;
        uint8_t m_segment_needs_resizing;
        uint32_t m_compression_info;
        s_s_element_t* m_ss;
        uint8_t m_has_bit_packed_sub_seg;
        c_s1_element_t* m_cs;
        std::string m_cs_tag_end;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t::c_p_element_t* m__parent;

    public:
        std::string cs_tag() const { return m_cs_tag; }
        uint64_t records() const { return m_records; }
        uint64_t one() const { return m_one; }
        uint32_t a_b_a_5_a() const { return m_a_b_a_5_a; }
        uint32_t iterator() const { return m_iterator; }
        uint64_t bookmark_bits_1_2_8() const { return m_bookmark_bits_1_2_8; }
        uint64_t storage_alloc_size() const { return m_storage_alloc_size; }
        uint64_t storage_used_size() const { return m_storage_used_size; }
        uint8_t segment_needs_resizing() const { return m_segment_needs_resizing; }
        uint32_t compression_info() const { return m_compression_info; }
        s_s_element_t* ss() const { return m_ss; }
        uint8_t has_bit_packed_sub_seg() const { return m_has_bit_packed_sub_seg; }
        c_s1_element_t* cs() const { return m_cs; }
        std::string cs_tag_end() const { return m_cs_tag_end; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t::c_p_element_t* _parent() const { return m__parent; }
    };

    class s_d_os_element_t : public kaitai::kstruct {

    public:

        s_d_os_element_t(kaitai::kstream* p__io, idfmeta_parser_t::block_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~s_d_os_element_t();

    private:
        std::string m_sdos_tag;
        c_s_d_os_element_t* m_csdos;
        std::string m_sdos_tag_end;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t::block_t* m__parent;

    public:
        std::string sdos_tag() const { return m_sdos_tag; }
        c_s_d_os_element_t* csdos() const { return m_csdos; }
        std::string sdos_tag_end() const { return m_sdos_tag_end; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t::block_t* _parent() const { return m__parent; }
    };

    class s_s_element_t : public kaitai::kstruct {

    public:

        s_s_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_s0_element_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~s_s_element_t();

    private:
        std::string m_ss_tag;
        uint64_t m_distinct_states;
        uint32_t m_min_data_id;
        uint32_t m_max_data_id;
        uint32_t m_original_min_segment_data_id;
        int64_t m_r_l_e_sort_order;
        uint64_t m_row_count;
        uint8_t m_has_nulls;
        uint64_t m_r_l_e_runs;
        uint64_t m_others_r_l_e_runs;
        std::string m_ss_tag_end;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t::c_s0_element_t* m__parent;

    public:
        std::string ss_tag() const { return m_ss_tag; }
        uint64_t distinct_states() const { return m_distinct_states; }
        uint32_t min_data_id() const { return m_min_data_id; }
        uint32_t max_data_id() const { return m_max_data_id; }
        uint32_t original_min_segment_data_id() const { return m_original_min_segment_data_id; }
        int64_t r_l_e_sort_order() const { return m_r_l_e_sort_order; }
        uint64_t row_count() const { return m_row_count; }
        uint8_t has_nulls() const { return m_has_nulls; }
        uint64_t r_l_e_runs() const { return m_r_l_e_runs; }
        uint64_t others_r_l_e_runs() const { return m_others_r_l_e_runs; }
        std::string ss_tag_end() const { return m_ss_tag_end; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t::c_s0_element_t* _parent() const { return m__parent; }
    };

    class block_t : public kaitai::kstruct {

    public:

        block_t(kaitai::kstream* p__io, idfmeta_parser_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~block_t();

    private:
        c_p_element_t* m_cp;
        s_d_os_element_t* m_sdos;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t* m__parent;

    public:
        c_p_element_t* cp() const { return m_cp; }
        s_d_os_element_t* sdos() const { return m_sdos; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t* _parent() const { return m__parent; }
    };

    class c_s_d_os1_element_t : public kaitai::kstruct {

    public:

        c_s_d_os1_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_s_d_os_element_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~c_s_d_os1_element_t();

    private:
        std::string m_csdos_tag;
        uint64_t m_sub_segment_offset;
        uint64_t m_sub_segment_size;
        std::string m_csdos_tag_end;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t::c_s_d_os_element_t* m__parent;

    public:
        std::string csdos_tag() const { return m_csdos_tag; }
        uint64_t sub_segment_offset() const { return m_sub_segment_offset; }
        uint64_t sub_segment_size() const { return m_sub_segment_size; }
        std::string csdos_tag_end() const { return m_csdos_tag_end; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t::c_s_d_os_element_t* _parent() const { return m__parent; }
    };

    class c_s_d_os_element_t : public kaitai::kstruct {

    public:

        c_s_d_os_element_t(kaitai::kstream* p__io, idfmeta_parser_t::s_d_os_element_t* p__parent = 0, idfmeta_parser_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~c_s_d_os_element_t();

    private:
        std::string m_csdos_tag;
        uint64_t m_zero_c_s_d_o;
        uint64_t m_primary_segment_size;
        c_s_d_os1_element_t* m_csdos;
        std::string m_csdos_tag_end;
        idfmeta_parser_t* m__root;
        idfmeta_parser_t::s_d_os_element_t* m__parent;

    public:
        std::string csdos_tag() const { return m_csdos_tag; }
        uint64_t zero_c_s_d_o() const { return m_zero_c_s_d_o; }
        uint64_t primary_segment_size() const { return m_primary_segment_size; }
        c_s_d_os1_element_t* csdos() const { return m_csdos; }
        std::string csdos_tag_end() const { return m_csdos_tag_end; }
        idfmeta_parser_t* _root() const { return m__root; }
        idfmeta_parser_t::s_d_os_element_t* _parent() const { return m__parent; }
    };

private:
    bool f_bit_width;
    int32_t m_bit_width;

public:
    int32_t bit_width();

private:
    block_t* m_blocks;
    idfmeta_parser_t* m__root;
    kaitai::kstruct* m__parent;

public:
    block_t* blocks() const { return m_blocks; }
    idfmeta_parser_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // IDFMETA_PARSER_H_

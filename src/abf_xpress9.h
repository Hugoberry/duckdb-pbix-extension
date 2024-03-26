#ifndef ABF_XPRESS9_H_
#define ABF_XPRESS9_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <vector>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

class abf_xpress9_t : public kaitai::kstruct {

public:
    class chunk_t;
    class node_t;
    class header_t;
    class huffman_flags_t;

    enum mtf_entries_enum_t {
        MTF_ENTRIES_ENUM_NO_ENTRIES = 0
    };

    abf_xpress9_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, abf_xpress9_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~abf_xpress9_t();

    class chunk_t : public kaitai::kstruct {

    public:

        chunk_t(kaitai::kstream* p__io, abf_xpress9_t* p__parent = 0, abf_xpress9_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~chunk_t();

    private:
        uint32_t m_uncompressed;
        uint32_t m_compressed;
        node_t* m_node;
        abf_xpress9_t* m__root;
        abf_xpress9_t* m__parent;
        std::string m__raw_node;
        kaitai::kstream* m__io__raw_node;

    public:
        uint32_t uncompressed() const { return m_uncompressed; }
        uint32_t compressed() const { return m_compressed; }
        node_t* node() const { return m_node; }
        abf_xpress9_t* _root() const { return m__root; }
        abf_xpress9_t* _parent() const { return m__parent; }
        std::string _raw_node() const { return m__raw_node; }
        kaitai::kstream* _io__raw_node() const { return m__io__raw_node; }
    };

    class node_t : public kaitai::kstruct {

    public:

        node_t(kaitai::kstream* p__io, abf_xpress9_t::chunk_t* p__parent = 0, abf_xpress9_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~node_t();

    private:
        header_t* m_header;
        std::string m_segments;
        abf_xpress9_t* m__root;
        abf_xpress9_t::chunk_t* m__parent;

    public:
        header_t* header() const { return m_header; }
        std::string segments() const { return m_segments; }
        abf_xpress9_t* _root() const { return m__root; }
        abf_xpress9_t::chunk_t* _parent() const { return m__parent; }
    };

    class header_t : public kaitai::kstruct {

    public:

        header_t(kaitai::kstream* p__io, abf_xpress9_t::node_t* p__parent = 0, abf_xpress9_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~header_t();

    private:
        std::string m_xpress_magic;
        uint32_t m_orig_size;
        uint32_t m_encoded_size;
        huffman_flags_t* m_huffman_table_flags;
        uint32_t m_zero;
        uint32_t m_session_signature;
        uint32_t m_block_index;
        uint32_t m_crc32;
        abf_xpress9_t* m__root;
        abf_xpress9_t::node_t* m__parent;

    public:

        /**
         * 3.708319535772664e+23
         */
        std::string xpress_magic() const { return m_xpress_magic; }
        uint32_t orig_size() const { return m_orig_size; }
        uint32_t encoded_size() const { return m_encoded_size; }
        huffman_flags_t* huffman_table_flags() const { return m_huffman_table_flags; }
        uint32_t zero() const { return m_zero; }
        uint32_t session_signature() const { return m_session_signature; }
        uint32_t block_index() const { return m_block_index; }
        uint32_t crc32() const { return m_crc32; }
        abf_xpress9_t* _root() const { return m__root; }
        abf_xpress9_t::node_t* _parent() const { return m__parent; }
    };

    class huffman_flags_t : public kaitai::kstruct {

    public:

        huffman_flags_t(kaitai::kstream* p__io, abf_xpress9_t::header_t* p__parent = 0, abf_xpress9_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~huffman_flags_t();

    private:
        uint64_t m_huffman_table_length;
        uint64_t m_log2_window_size;
        mtf_entries_enum_t m_mtf_entries;
        bool m_min_ptr_match_length_flag;
        bool m_min_mtf_match_length_flag;
        uint64_t m_reserved_zero;
        abf_xpress9_t* m__root;
        abf_xpress9_t::header_t* m__parent;

    public:

        /**
         * Length of encoded Huffman tables in bits
         */
        uint64_t huffman_table_length() const { return m_huffman_table_length; }

        /**
         * log2 of window size (16 .. 23)
         */
        uint64_t log2_window_size() const { return m_log2_window_size; }

        /**
         * Number of MTF entries
         */
        mtf_entries_enum_t mtf_entries() const { return m_mtf_entries; }

        /**
         * Min pointer match length flag (0 for length 3, 1 for length 4)
         */
        bool min_ptr_match_length_flag() const { return m_min_ptr_match_length_flag; }

        /**
         * Min MTF match length flag (0 for length 2, 1 for length 3)
         */
        bool min_mtf_match_length_flag() const { return m_min_mtf_match_length_flag; }

        /**
         * reserved, must be 0
         */
        uint64_t reserved_zero() const { return m_reserved_zero; }
        abf_xpress9_t* _root() const { return m__root; }
        abf_xpress9_t::header_t* _parent() const { return m__parent; }
    };

private:
    std::string m_signature;
    std::vector<chunk_t*>* m_chunks;
    abf_xpress9_t* m__root;
    kaitai::kstruct* m__parent;

public:

    /**
     * This backup was created using Xpress9 compression.
     */
    std::string signature() const { return m_signature; }
    std::vector<chunk_t*>* chunks() const { return m_chunks; }
    abf_xpress9_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // ABF_XPRESS9_H_

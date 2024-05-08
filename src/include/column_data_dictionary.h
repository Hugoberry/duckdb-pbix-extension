#ifndef COLUMN_DATA_DICTIONARY_H_
#define COLUMN_DATA_DICTIONARY_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <vector>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

class column_data_dictionary_t : public kaitai::kstruct {

public:
    class string_record_handle_t;
    class string_data_t;
    class hash_info_t;
    class vector_of_vectors_t;
    class compressed_strings_t;
    class page_layout_t;
    class dictionary_page_t;
    class other_record_handle_t;
    class uncompressed_strings_t;
    class number_data_t;
    class dictionary_record_handles_vector_t;

    enum dictionary_types_t {
        DICTIONARY_TYPES_XM_TYPE_INVALID = -1,
        DICTIONARY_TYPES_XM_TYPE_LONG = 0,
        DICTIONARY_TYPES_XM_TYPE_REAL = 1,
        DICTIONARY_TYPES_XM_TYPE_STRING = 2
    };

    column_data_dictionary_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, column_data_dictionary_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~column_data_dictionary_t();

    class string_record_handle_t : public kaitai::kstruct {

    public:

        string_record_handle_t(kaitai::kstream* p__io, column_data_dictionary_t::dictionary_record_handles_vector_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~string_record_handle_t();

    private:
        uint32_t m_bit_or_byte_offset;
        uint32_t m_page_id;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t::dictionary_record_handles_vector_t* m__parent;

    public:
        uint32_t bit_or_byte_offset() const { return m_bit_or_byte_offset; }
        uint32_t page_id() const { return m_page_id; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t::dictionary_record_handles_vector_t* _parent() const { return m__parent; }
    };

    class string_data_t : public kaitai::kstruct {

    public:

        string_data_t(kaitai::kstream* p__io, column_data_dictionary_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~string_data_t();

    private:
        page_layout_t* m_page_layout_information;
        std::vector<dictionary_page_t*>* m_dictionary_pages;
        dictionary_record_handles_vector_t* m_dictionary_record_handles_vector_info;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t* m__parent;

    public:
        page_layout_t* page_layout_information() const { return m_page_layout_information; }
        std::vector<dictionary_page_t*>* dictionary_pages() const { return m_dictionary_pages; }
        dictionary_record_handles_vector_t* dictionary_record_handles_vector_info() const { return m_dictionary_record_handles_vector_info; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t* _parent() const { return m__parent; }
    };

    class hash_info_t : public kaitai::kstruct {

    public:

        hash_info_t(kaitai::kstream* p__io, column_data_dictionary_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~hash_info_t();

    private:
        std::vector<int32_t>* m_hash_elements;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t* m__parent;

    public:
        std::vector<int32_t>* hash_elements() const { return m_hash_elements; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t* _parent() const { return m__parent; }
    };

    class vector_of_vectors_t : public kaitai::kstruct {

    public:

        vector_of_vectors_t(kaitai::kstream* p__io, column_data_dictionary_t::number_data_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~vector_of_vectors_t();

    private:
        bool f_is_int32;
        bool m_is_int32;

    public:
        bool is_int32();

    private:
        bool f_is_int64;
        bool m_is_int64;

    public:
        bool is_int64();

    private:
        bool f_is_float64;
        bool m_is_float64;

    public:
        bool is_float64();

    private:
        bool f_data_type_id;
        std::string m_data_type_id;

    public:
        std::string data_type_id();

    private:
        uint64_t m_num_values;
        uint32_t m_element_size;
        std::vector<double>* m_values;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t::number_data_t* m__parent;

    public:
        uint64_t num_values() const { return m_num_values; }
        uint32_t element_size() const { return m_element_size; }
        std::vector<double>* values() const { return m_values; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t::number_data_t* _parent() const { return m__parent; }
    };

    class compressed_strings_t : public kaitai::kstruct {

    public:

        compressed_strings_t(kaitai::kstream* p__io, column_data_dictionary_t::dictionary_page_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~compressed_strings_t();

    private:
        uint32_t m_store_total_bits;
        uint32_t m_character_set_type_identifier;
        uint64_t m_len_compressed_string_buffer;
        uint8_t m_character_set_used;
        uint32_t m_ui_decode_bits;
        std::vector<uint8_t>* m_encode_array;
        uint64_t m_ui64_buffer_size;
        std::string m_compressed_string_buffer;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t::dictionary_page_t* m__parent;

    public:
        uint32_t store_total_bits() const { return m_store_total_bits; }
        uint32_t character_set_type_identifier() const { return m_character_set_type_identifier; }
        uint64_t len_compressed_string_buffer() const { return m_len_compressed_string_buffer; }
        uint8_t character_set_used() const { return m_character_set_used; }
        uint32_t ui_decode_bits() const { return m_ui_decode_bits; }
        std::vector<uint8_t>* encode_array() const { return m_encode_array; }
        uint64_t ui64_buffer_size() const { return m_ui64_buffer_size; }
        std::string compressed_string_buffer() const { return m_compressed_string_buffer; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t::dictionary_page_t* _parent() const { return m__parent; }
    };

    class page_layout_t : public kaitai::kstruct {

    public:

        page_layout_t(kaitai::kstream* p__io, column_data_dictionary_t::string_data_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~page_layout_t();

    private:
        int64_t m_store_string_count;
        int8_t m_f_store_compressed;
        int64_t m_store_longest_string;
        int64_t m_store_page_count;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t::string_data_t* m__parent;

    public:
        int64_t store_string_count() const { return m_store_string_count; }
        int8_t f_store_compressed() const { return m_f_store_compressed; }
        int64_t store_longest_string() const { return m_store_longest_string; }
        int64_t store_page_count() const { return m_store_page_count; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t::string_data_t* _parent() const { return m__parent; }
    };

    class dictionary_page_t : public kaitai::kstruct {

    public:

        dictionary_page_t(kaitai::kstream* p__io, column_data_dictionary_t::string_data_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~dictionary_page_t();

    private:
        uint64_t m_page_mask;
        uint8_t m_page_contains_nulls;
        uint64_t m_page_start_index;
        uint64_t m_page_string_count;
        uint8_t m_page_compressed;
        std::string m_string_store_begin_mark;
        kaitai::kstruct* m_string_store;
        bool n_string_store;

    public:
        bool _is_null_string_store() { string_store(); return n_string_store; };

    private:
        std::string m_string_store_end_mark;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t::string_data_t* m__parent;

    public:
        uint64_t page_mask() const { return m_page_mask; }
        uint8_t page_contains_nulls() const { return m_page_contains_nulls; }
        uint64_t page_start_index() const { return m_page_start_index; }
        uint64_t page_string_count() const { return m_page_string_count; }
        uint8_t page_compressed() const { return m_page_compressed; }
        std::string string_store_begin_mark() const { return m_string_store_begin_mark; }
        kaitai::kstruct* string_store() const { return m_string_store; }
        std::string string_store_end_mark() const { return m_string_store_end_mark; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t::string_data_t* _parent() const { return m__parent; }
    };

    class other_record_handle_t : public kaitai::kstruct {

    public:

        other_record_handle_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~other_record_handle_t();

    private:
        uint32_t m_bit_or_byte_offset;
        column_data_dictionary_t* m__root;
        kaitai::kstruct* m__parent;

    public:
        uint32_t bit_or_byte_offset() const { return m_bit_or_byte_offset; }
        column_data_dictionary_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }
    };

    class uncompressed_strings_t : public kaitai::kstruct {

    public:

        uncompressed_strings_t(kaitai::kstream* p__io, column_data_dictionary_t::dictionary_page_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~uncompressed_strings_t();

    private:
        uint64_t m_remaining_store_available;
        uint64_t m_buffer_used_characters;
        uint64_t m_allocation_size;
        std::string m_uncompressed_character_buffer;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t::dictionary_page_t* m__parent;

    public:
        uint64_t remaining_store_available() const { return m_remaining_store_available; }
        uint64_t buffer_used_characters() const { return m_buffer_used_characters; }
        uint64_t allocation_size() const { return m_allocation_size; }
        std::string uncompressed_character_buffer() const { return m_uncompressed_character_buffer; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t::dictionary_page_t* _parent() const { return m__parent; }
    };

    class number_data_t : public kaitai::kstruct {

    public:

        number_data_t(kaitai::kstream* p__io, column_data_dictionary_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~number_data_t();

    private:
        vector_of_vectors_t* m_vector_of_vectors_info;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t* m__parent;

    public:
        vector_of_vectors_t* vector_of_vectors_info() const { return m_vector_of_vectors_info; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t* _parent() const { return m__parent; }
    };

    class dictionary_record_handles_vector_t : public kaitai::kstruct {

    public:

        dictionary_record_handles_vector_t(kaitai::kstream* p__io, column_data_dictionary_t::string_data_t* p__parent = 0, column_data_dictionary_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~dictionary_record_handles_vector_t();

    private:
        uint64_t m_num_vector_of_record_handle_structures;
        std::string m_element_size;
        std::vector<string_record_handle_t*>* m_vector_of_record_handle_structures;
        column_data_dictionary_t* m__root;
        column_data_dictionary_t::string_data_t* m__parent;

    public:
        uint64_t num_vector_of_record_handle_structures() const { return m_num_vector_of_record_handle_structures; }
        std::string element_size() const { return m_element_size; }
        std::vector<string_record_handle_t*>* vector_of_record_handle_structures() const { return m_vector_of_record_handle_structures; }
        column_data_dictionary_t* _root() const { return m__root; }
        column_data_dictionary_t::string_data_t* _parent() const { return m__parent; }
    };

private:
    dictionary_types_t m_dictionary_type;
    hash_info_t* m_hash_information;
    kaitai::kstruct* m_data;
    bool n_data;

public:
    bool _is_null_data() { data(); return n_data; };

private:
    column_data_dictionary_t* m__root;
    kaitai::kstruct* m__parent;

public:
    dictionary_types_t dictionary_type() const { return m_dictionary_type; }
    hash_info_t* hash_information() const { return m_hash_information; }
    kaitai::kstruct* data() const { return m_data; }
    column_data_dictionary_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // COLUMN_DATA_DICTIONARY_H_

#ifndef COLUMN_DATA_HIDX_H_
#define COLUMN_DATA_HIDX_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <vector>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

class column_data_hidx_t : public kaitai::kstruct {

public:
    class hash_statistics_type_t;
    class hash_bin_t;
    class hash_entry_t;

    column_data_hidx_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, column_data_hidx_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~column_data_hidx_t();

    class hash_statistics_type_t : public kaitai::kstruct {

    public:

        hash_statistics_type_t(kaitai::kstream* p__io, column_data_hidx_t* p__parent = 0, column_data_hidx_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~hash_statistics_type_t();

    private:
        uint64_t m_number_of_elements;
        uint64_t m_number_of_bins;
        uint64_t m_number_of_used_bins;
        uint64_t m_fast_access_elements;
        uint64_t m_locals_size_per_bin;
        uint64_t m_maximum_chain;
        uint64_t m_element_count;
        uint32_t m_element_size;
        std::vector<uint64_t>* m_histogram_vector;
        column_data_hidx_t* m__root;
        column_data_hidx_t* m__parent;

    public:
        uint64_t number_of_elements() const { return m_number_of_elements; }
        uint64_t number_of_bins() const { return m_number_of_bins; }
        uint64_t number_of_used_bins() const { return m_number_of_used_bins; }
        uint64_t fast_access_elements() const { return m_fast_access_elements; }
        uint64_t locals_size_per_bin() const { return m_locals_size_per_bin; }
        uint64_t maximum_chain() const { return m_maximum_chain; }
        uint64_t element_count() const { return m_element_count; }
        uint32_t element_size() const { return m_element_size; }
        std::vector<uint64_t>* histogram_vector() const { return m_histogram_vector; }
        column_data_hidx_t* _root() const { return m__root; }
        column_data_hidx_t* _parent() const { return m__parent; }
    };

    class hash_bin_t : public kaitai::kstruct {

    public:

        hash_bin_t(kaitai::kstream* p__io, column_data_hidx_t* p__parent = 0, column_data_hidx_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~hash_bin_t();

    private:
        uint64_t m_m_rg_chain;
        uint32_t m_m_count;
        std::vector<hash_entry_t*>* m_m_rg_local_entries;
        uint32_t m_padding;
        column_data_hidx_t* m__root;
        column_data_hidx_t* m__parent;

    public:
        uint64_t m_rg_chain() const { return m_m_rg_chain; }
        uint32_t m_count() const { return m_m_count; }
        std::vector<hash_entry_t*>* m_rg_local_entries() const { return m_m_rg_local_entries; }
        uint32_t padding() const { return m_padding; }
        column_data_hidx_t* _root() const { return m__root; }
        column_data_hidx_t* _parent() const { return m__parent; }
    };

    class hash_entry_t : public kaitai::kstruct {

    public:

        hash_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, column_data_hidx_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~hash_entry_t();

    private:
        uint32_t m_m_hash;
        uint32_t m_m_key;
        column_data_hidx_t* m__root;
        kaitai::kstruct* m__parent;

    public:
        uint32_t m_hash() const { return m_m_hash; }
        uint32_t m_key() const { return m_m_key; }
        column_data_hidx_t* _root() const { return m__root; }
        kaitai::kstruct* _parent() const { return m__parent; }
    };

private:
    int32_t m_hash_algorithm;
    uint32_t m_hash_entry_size;
    uint32_t m_hash_bin_size;
    uint32_t m_local_entry_count;
    uint64_t m_c_bins;
    int64_t m_number_of_records;
    int64_t m_current_mask;
    uint8_t m_hash_stats;
    hash_statistics_type_t* m_hash_statistics;
    bool n_hash_statistics;

public:
    bool _is_null_hash_statistics() { hash_statistics(); return n_hash_statistics; };

private:
    std::vector<hash_bin_t*>* m_hash_bin_entries;
    uint64_t m_overflow_hash_entries_count;
    std::vector<hash_entry_t*>* m_overflow_hash_entries;
    column_data_hidx_t* m__root;
    kaitai::kstruct* m__parent;
    std::vector<std::string>* m__raw_hash_bin_entries;
    std::vector<kaitai::kstream*>* m__io__raw_hash_bin_entries;

public:
    int32_t hash_algorithm() const { return m_hash_algorithm; }
    uint32_t hash_entry_size() const { return m_hash_entry_size; }
    uint32_t hash_bin_size() const { return m_hash_bin_size; }
    uint32_t local_entry_count() const { return m_local_entry_count; }
    uint64_t c_bins() const { return m_c_bins; }
    int64_t number_of_records() const { return m_number_of_records; }
    int64_t current_mask() const { return m_current_mask; }
    uint8_t hash_stats() const { return m_hash_stats; }
    hash_statistics_type_t* hash_statistics() const { return m_hash_statistics; }
    std::vector<hash_bin_t*>* hash_bin_entries() const { return m_hash_bin_entries; }
    uint64_t overflow_hash_entries_count() const { return m_overflow_hash_entries_count; }
    std::vector<hash_entry_t*>* overflow_hash_entries() const { return m_overflow_hash_entries; }
    column_data_hidx_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
    std::vector<std::string>* _raw_hash_bin_entries() const { return m__raw_hash_bin_entries; }
    std::vector<kaitai::kstream*>* _io__raw_hash_bin_entries() const { return m__io__raw_hash_bin_entries; }
};

#endif  // COLUMN_DATA_HIDX_H_

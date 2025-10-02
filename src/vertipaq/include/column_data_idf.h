#ifndef COLUMN_DATA_IDF_H_
#define COLUMN_DATA_IDF_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "kaitai/kaitaistruct.h"
#include <stdint.h>
#include <vector>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

class column_data_idf_t : public kaitai::kstruct {

public:
    class segment_t;
    class segment_entry_t;

    column_data_idf_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, column_data_idf_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~column_data_idf_t();

    class segment_t : public kaitai::kstruct {

    public:

        segment_t(kaitai::kstream* p__io, column_data_idf_t* p__parent = 0, column_data_idf_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~segment_t();

    private:
        uint64_t m_primary_segment_size;
        std::vector<segment_entry_t*>* m_primary_segment;
        uint64_t m_sub_segment_size;
        std::vector<uint64_t>* m_sub_segment;
        column_data_idf_t* m__root;
        column_data_idf_t* m__parent;

    public:
        uint64_t primary_segment_size() const { return m_primary_segment_size; }
        std::vector<segment_entry_t*>* primary_segment() const { return m_primary_segment; }
        uint64_t sub_segment_size() const { return m_sub_segment_size; }
        std::vector<uint64_t>* sub_segment() const { return m_sub_segment; }
        column_data_idf_t* _root() const { return m__root; }
        column_data_idf_t* _parent() const { return m__parent; }
    };

    class segment_entry_t : public kaitai::kstruct {

    public:

        segment_entry_t(kaitai::kstream* p__io, column_data_idf_t::segment_t* p__parent = 0, column_data_idf_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~segment_entry_t();

    private:
        uint32_t m_data_value;
        uint32_t m_repeat_value;
        column_data_idf_t* m__root;
        column_data_idf_t::segment_t* m__parent;

    public:
        uint32_t data_value() const { return m_data_value; }
        uint32_t repeat_value() const { return m_repeat_value; }
        column_data_idf_t* _root() const { return m__root; }
        column_data_idf_t::segment_t* _parent() const { return m__parent; }
    };

private:
    std::vector<segment_t*>* m_segments;
    column_data_idf_t* m__root;
    kaitai::kstruct* m__parent;

public:
    std::vector<segment_t*>* segments() const { return m_segments; }
    column_data_idf_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // COLUMN_DATA_IDF_H_

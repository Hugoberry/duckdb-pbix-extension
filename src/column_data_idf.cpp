// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "column_data_idf.h"

column_data_idf_t::column_data_idf_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, column_data_idf_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_segments = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_idf_t::_read() {
    m_segments = new std::vector<segment_t*>();
    {
        int i = 0;
        while (!m__io->is_eof()) {
            m_segments->push_back(new segment_t(m__io, this, m__root));
            i++;
        }
    }
}

column_data_idf_t::~column_data_idf_t() {
    _clean_up();
}

void column_data_idf_t::_clean_up() {
    if (m_segments) {
        for (std::vector<segment_t*>::iterator it = m_segments->begin(); it != m_segments->end(); ++it) {
            delete *it;
        }
        delete m_segments; m_segments = 0;
    }
}

column_data_idf_t::segment_t::segment_t(kaitai::kstream* p__io, column_data_idf_t* p__parent, column_data_idf_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_primary_segment = 0;
    m_sub_segment = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_idf_t::segment_t::_read() {
    m_primary_segment_size = m__io->read_u8le();
    m_primary_segment = new std::vector<segment_entry_t*>();
    const int l_primary_segment = primary_segment_size();
    for (int i = 0; i < l_primary_segment; i++) {
        m_primary_segment->push_back(new segment_entry_t(m__io, this, m__root));
    }
    m_sub_segment_size = m__io->read_u8le();
    m_sub_segment = new std::vector<uint64_t>();
    const int l_sub_segment = sub_segment_size();
    for (int i = 0; i < l_sub_segment; i++) {
        m_sub_segment->push_back(m__io->read_u8le());
    }
}

column_data_idf_t::segment_t::~segment_t() {
    _clean_up();
}

void column_data_idf_t::segment_t::_clean_up() {
    if (m_primary_segment) {
        for (std::vector<segment_entry_t*>::iterator it = m_primary_segment->begin(); it != m_primary_segment->end(); ++it) {
            delete *it;
        }
        delete m_primary_segment; m_primary_segment = 0;
    }
    if (m_sub_segment) {
        delete m_sub_segment; m_sub_segment = 0;
    }
}

column_data_idf_t::segment_entry_t::segment_entry_t(kaitai::kstream* p__io, column_data_idf_t::segment_t* p__parent, column_data_idf_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_idf_t::segment_entry_t::_read() {
    m_data_value = m__io->read_u4le();
    m_repeat_value = m__io->read_u4le();
}

column_data_idf_t::segment_entry_t::~segment_entry_t() {
    _clean_up();
}

void column_data_idf_t::segment_entry_t::_clean_up() {
}

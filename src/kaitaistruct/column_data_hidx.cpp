// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "column_data_hidx.h"

column_data_hidx_t::column_data_hidx_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, column_data_hidx_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_hash_statistics = 0;
    m_hash_bin_entries = 0;
    m__raw_hash_bin_entries = 0;
    m__io__raw_hash_bin_entries = 0;
    m_overflow_hash_entries = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_hidx_t::_read() {
    m_hash_algorithm = m__io->read_s4le();
    m_hash_entry_size = m__io->read_u4le();
    m_hash_bin_size = m__io->read_u4le();
    m_local_entry_count = m__io->read_u4le();
    m_c_bins = m__io->read_u8le();
    m_number_of_records = m__io->read_s8le();
    m_current_mask = m__io->read_s8le();
    m_hash_stats = m__io->read_u1();
    n_hash_statistics = true;
    if (hash_stats() != 0) {
        n_hash_statistics = false;
        m_hash_statistics = new hash_statistics_type_t(m__io, this, m__root);
    }
    m__raw_hash_bin_entries = new std::vector<std::string>();
    m__io__raw_hash_bin_entries = new std::vector<kaitai::kstream*>();
    m_hash_bin_entries = new std::vector<hash_bin_t*>();
    const int l_hash_bin_entries = c_bins();
    for (int i = 0; i < l_hash_bin_entries; i++) {
        m__raw_hash_bin_entries->push_back(m__io->read_bytes(hash_bin_size()));
        kaitai::kstream* io__raw_hash_bin_entries = new kaitai::kstream(m__raw_hash_bin_entries->at(m__raw_hash_bin_entries->size() - 1));
        m__io__raw_hash_bin_entries->push_back(io__raw_hash_bin_entries);
        m_hash_bin_entries->push_back(new hash_bin_t(io__raw_hash_bin_entries, this, m__root));
    }
    m_overflow_hash_entries_count = m__io->read_u8le();
    m_overflow_hash_entries = new std::vector<hash_entry_t*>();
    const int l_overflow_hash_entries = overflow_hash_entries_count();
    for (int i = 0; i < l_overflow_hash_entries; i++) {
        m_overflow_hash_entries->push_back(new hash_entry_t(m__io, this, m__root));
    }
}

column_data_hidx_t::~column_data_hidx_t() {
    _clean_up();
}

void column_data_hidx_t::_clean_up() {
    if (!n_hash_statistics) {
        if (m_hash_statistics) {
            delete m_hash_statistics; m_hash_statistics = 0;
        }
    }
    if (m__raw_hash_bin_entries) {
        delete m__raw_hash_bin_entries; m__raw_hash_bin_entries = 0;
    }
    if (m__io__raw_hash_bin_entries) {
        for (std::vector<kaitai::kstream*>::iterator it = m__io__raw_hash_bin_entries->begin(); it != m__io__raw_hash_bin_entries->end(); ++it) {
            delete *it;
        }
        delete m__io__raw_hash_bin_entries; m__io__raw_hash_bin_entries = 0;
    }
    if (m_hash_bin_entries) {
        for (std::vector<hash_bin_t*>::iterator it = m_hash_bin_entries->begin(); it != m_hash_bin_entries->end(); ++it) {
            delete *it;
        }
        delete m_hash_bin_entries; m_hash_bin_entries = 0;
    }
    if (m_overflow_hash_entries) {
        for (std::vector<hash_entry_t*>::iterator it = m_overflow_hash_entries->begin(); it != m_overflow_hash_entries->end(); ++it) {
            delete *it;
        }
        delete m_overflow_hash_entries; m_overflow_hash_entries = 0;
    }
}

column_data_hidx_t::hash_statistics_type_t::hash_statistics_type_t(kaitai::kstream* p__io, column_data_hidx_t* p__parent, column_data_hidx_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_histogram_vector = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_hidx_t::hash_statistics_type_t::_read() {
    m_number_of_elements = m__io->read_u8le();
    m_number_of_bins = m__io->read_u8le();
    m_number_of_used_bins = m__io->read_u8le();
    m_fast_access_elements = m__io->read_u8le();
    m_locals_size_per_bin = m__io->read_u8le();
    m_maximum_chain = m__io->read_u8le();
    m_element_count = m__io->read_u8le();
    m_element_size = m__io->read_u4le();
    m_histogram_vector = new std::vector<uint64_t>();
    const int l_histogram_vector = element_count();
    for (int i = 0; i < l_histogram_vector; i++) {
        switch (element_size()) {
        case 4: {
            m_histogram_vector->push_back(m__io->read_u4le());
            break;
        }
        case 8: {
            m_histogram_vector->push_back(m__io->read_u8le());
            break;
        }
        }
    }
}

column_data_hidx_t::hash_statistics_type_t::~hash_statistics_type_t() {
    _clean_up();
}

void column_data_hidx_t::hash_statistics_type_t::_clean_up() {
    if (m_histogram_vector) {
        delete m_histogram_vector; m_histogram_vector = 0;
    }
}

column_data_hidx_t::hash_bin_t::hash_bin_t(kaitai::kstream* p__io, column_data_hidx_t* p__parent, column_data_hidx_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_m_rg_local_entries = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_hidx_t::hash_bin_t::_read() {
    m_m_rg_chain = m__io->read_u8le();
    m_m_count = m__io->read_u4le();
    m_m_rg_local_entries = new std::vector<hash_entry_t*>();
    const int l_m_rg_local_entries = _root()->local_entry_count();
    for (int i = 0; i < l_m_rg_local_entries; i++) {
        m_m_rg_local_entries->push_back(new hash_entry_t(m__io, this, m__root));
    }
    m_padding = m__io->read_u4le();
}

column_data_hidx_t::hash_bin_t::~hash_bin_t() {
    _clean_up();
}

void column_data_hidx_t::hash_bin_t::_clean_up() {
    if (m_m_rg_local_entries) {
        for (std::vector<hash_entry_t*>::iterator it = m_m_rg_local_entries->begin(); it != m_m_rg_local_entries->end(); ++it) {
            delete *it;
        }
        delete m_m_rg_local_entries; m_m_rg_local_entries = 0;
    }
}

column_data_hidx_t::hash_entry_t::hash_entry_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, column_data_hidx_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_hidx_t::hash_entry_t::_read() {
    m_m_hash = m__io->read_u4le();
    m_m_key = m__io->read_u4le();
}

column_data_hidx_t::hash_entry_t::~hash_entry_t() {
    _clean_up();
}

void column_data_hidx_t::hash_entry_t::_clean_up() {
}

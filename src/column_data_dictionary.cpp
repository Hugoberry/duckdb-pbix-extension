// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "column_data_dictionary.h"

column_data_dictionary_t::column_data_dictionary_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_hash_information = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::_read() {
    m_dictionary_type = static_cast<column_data_dictionary_t::dictionary_types_t>(m__io->read_s4le());
    m_hash_information = new hash_info_t(m__io, this, m__root);
    n_data = true;
    switch (dictionary_type()) {
    case column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_STRING: {
        n_data = false;
        m_data = new string_data_t(m__io, this, m__root);
        break;
    }
    case column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_LONG: {
        n_data = false;
        m_data = new number_data_t(m__io, this, m__root);
        break;
    }
    case column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_REAL: {
        n_data = false;
        m_data = new number_data_t(m__io, this, m__root);
        break;
    }
    }
}

column_data_dictionary_t::~column_data_dictionary_t() {
    _clean_up();
}

void column_data_dictionary_t::_clean_up() {
    if (m_hash_information) {
        delete m_hash_information; m_hash_information = 0;
    }
    if (!n_data) {
        if (m_data) {
            delete m_data; m_data = 0;
        }
    }
}

column_data_dictionary_t::string_data_t::string_data_t(kaitai::kstream* p__io, column_data_dictionary_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_page_layout_information = 0;
    m_dictionary_pages = 0;
    m_dictionary_record_handles_vector_info = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::string_data_t::_read() {
    m_page_layout_information = new page_layout_t(m__io, this, m__root);
    m_dictionary_pages = new dictionary_page_t(m__io, this, m__root);
    m_dictionary_record_handles_vector_info = new dictionary_record_handles_vector_t(m__io, this, m__root);
}

column_data_dictionary_t::string_data_t::~string_data_t() {
    _clean_up();
}

void column_data_dictionary_t::string_data_t::_clean_up() {
    if (m_page_layout_information) {
        delete m_page_layout_information; m_page_layout_information = 0;
    }
    if (m_dictionary_pages) {
        delete m_dictionary_pages; m_dictionary_pages = 0;
    }
    if (m_dictionary_record_handles_vector_info) {
        delete m_dictionary_record_handles_vector_info; m_dictionary_record_handles_vector_info = 0;
    }
}

column_data_dictionary_t::string_store_section_t::string_store_section_t(kaitai::kstream* p__io, column_data_dictionary_t::dictionary_page_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::string_store_section_t::_read() {
    m_remaining_store_available = m__io->read_u8le();
    m_buffer_used_characters = m__io->read_u8le();
    m_allocation_size = m__io->read_u8le();
    m_uncompressed_character_buffer = kaitai::kstream::bytes_to_str(m__io->read_bytes(allocation_size()), "UTF-16LE");
}

column_data_dictionary_t::string_store_section_t::~string_store_section_t() {
    _clean_up();
}

void column_data_dictionary_t::string_store_section_t::_clean_up() {
}

column_data_dictionary_t::hash_info_t::hash_info_t(kaitai::kstream* p__io, column_data_dictionary_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_hash_elements = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::hash_info_t::_read() {
    m_hash_elements = new std::vector<int32_t>();
    const int l_hash_elements = 6;
    for (int i = 0; i < l_hash_elements; i++) {
        m_hash_elements->push_back(m__io->read_s4le());
    }
}

column_data_dictionary_t::hash_info_t::~hash_info_t() {
    _clean_up();
}

void column_data_dictionary_t::hash_info_t::_clean_up() {
    if (m_hash_elements) {
        delete m_hash_elements; m_hash_elements = 0;
    }
}

column_data_dictionary_t::vector_of_vectors_t::vector_of_vectors_t(kaitai::kstream* p__io, column_data_dictionary_t::number_data_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_values = 0;
    f_is_int32 = false;
    f_is_int64 = false;
    f_is_float64 = false;
    f_data_type_id = false;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::vector_of_vectors_t::_read() {
    m_element_count = m__io->read_u8le();
    m_element_size = m__io->read_u4le();
    m_values = new std::vector<double>();
    const int l_values = element_count();
    for (int i = 0; i < l_values; i++) {
        {
            std::string on = data_type_id();
            if (on == std::string("int32")) {
                m_values->push_back(m__io->read_s4le());
            }
            else if (on == std::string("int64")) {
                m_values->push_back(m__io->read_s8le());
            }
            else if (on == std::string("float64")) {
                m_values->push_back(m__io->read_f8le());
            }
        }
    }
}

column_data_dictionary_t::vector_of_vectors_t::~vector_of_vectors_t() {
    _clean_up();
}

void column_data_dictionary_t::vector_of_vectors_t::_clean_up() {
    if (m_values) {
        delete m_values; m_values = 0;
    }
}

bool column_data_dictionary_t::vector_of_vectors_t::is_int32() {
    if (f_is_int32)
        return m_is_int32;
    m_is_int32 = element_size() == 4;
    f_is_int32 = true;
    return m_is_int32;
}

bool column_data_dictionary_t::vector_of_vectors_t::is_int64() {
    if (f_is_int64)
        return m_is_int64;
    m_is_int64 =  ((element_size() == 8) && (_root()->dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_LONG)) ;
    f_is_int64 = true;
    return m_is_int64;
}

bool column_data_dictionary_t::vector_of_vectors_t::is_float64() {
    if (f_is_float64)
        return m_is_float64;
    m_is_float64 =  ((element_size() == 8) && (_root()->dictionary_type() == column_data_dictionary_t::DICTIONARY_TYPES_XM_TYPE_REAL)) ;
    f_is_float64 = true;
    return m_is_float64;
}

std::string column_data_dictionary_t::vector_of_vectors_t::data_type_id() {
    if (f_data_type_id)
        return m_data_type_id;
    m_data_type_id = ((is_int32()) ? (std::string("int32")) : (((is_int64()) ? (std::string("int64")) : (std::string("float64")))));
    f_data_type_id = true;
    return m_data_type_id;
}

column_data_dictionary_t::page_layout_t::page_layout_t(kaitai::kstream* p__io, column_data_dictionary_t::string_data_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::page_layout_t::_read() {
    m_store_string_count = m__io->read_s8le();
    m_f_store_compressed = m__io->read_s1();
    m_store_longest_string = m__io->read_s8le();
    m_store_page_count = m__io->read_s8le();
}

column_data_dictionary_t::page_layout_t::~page_layout_t() {
    _clean_up();
}

void column_data_dictionary_t::page_layout_t::_clean_up() {
}

column_data_dictionary_t::dictionary_page_t::dictionary_page_t(kaitai::kstream* p__io, column_data_dictionary_t::string_data_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_string_store = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::dictionary_page_t::_read() {
    m_page_mask = m__io->read_u8le();
    m_page_contains_nulls = m__io->read_u1();
    m_page_start_index = m__io->read_u8le();
    m_page_string_count = m__io->read_u8le();
    m_page_compressed = m__io->read_u1();
    m_string_store_begin_mark = m__io->read_u4le();
    m_string_store = new string_store_section_t(m__io, this, m__root);
    m_string_store_end_mark = m__io->read_u4le();
}

column_data_dictionary_t::dictionary_page_t::~dictionary_page_t() {
    _clean_up();
}

void column_data_dictionary_t::dictionary_page_t::_clean_up() {
    if (m_string_store) {
        delete m_string_store; m_string_store = 0;
    }
}

column_data_dictionary_t::number_data_t::number_data_t(kaitai::kstream* p__io, column_data_dictionary_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_vector_of_vectors_info = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::number_data_t::_read() {
    m_vector_of_vectors_info = new vector_of_vectors_t(m__io, this, m__root);
}

column_data_dictionary_t::number_data_t::~number_data_t() {
    _clean_up();
}

void column_data_dictionary_t::number_data_t::_clean_up() {
    if (m_vector_of_vectors_info) {
        delete m_vector_of_vectors_info; m_vector_of_vectors_info = 0;
    }
}

column_data_dictionary_t::dictionary_record_handles_vector_t::dictionary_record_handles_vector_t(kaitai::kstream* p__io, column_data_dictionary_t::string_data_t* p__parent, column_data_dictionary_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_vector_of_record_handle_structures = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void column_data_dictionary_t::dictionary_record_handles_vector_t::_read() {
    m_element_count = m__io->read_u8le();
    m_element_size = m__io->read_u4le();
    m_vector_of_record_handle_structures = new std::vector<uint64_t>();
    const int l_vector_of_record_handle_structures = element_count();
    for (int i = 0; i < l_vector_of_record_handle_structures; i++) {
        switch (element_size()) {
        case 4: {
            m_vector_of_record_handle_structures->push_back(m__io->read_u4le());
            break;
        }
        case 8: {
            m_vector_of_record_handle_structures->push_back(m__io->read_u8le());
            break;
        }
        }
    }
}

column_data_dictionary_t::dictionary_record_handles_vector_t::~dictionary_record_handles_vector_t() {
    _clean_up();
}

void column_data_dictionary_t::dictionary_record_handles_vector_t::_clean_up() {
    if (m_vector_of_record_handle_structures) {
        delete m_vector_of_record_handle_structures; m_vector_of_record_handle_structures = 0;
    }
}

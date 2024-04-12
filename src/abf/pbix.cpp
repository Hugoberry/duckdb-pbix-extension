// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "pbix.h"
#include "kaitai/exceptions.h"

pbix_t::pbix_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_sections = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::_read() {
    m_sections = new std::vector<pk_section_t*>();
    {
        int i = 0;
        while (!m__io->is_eof()) {
            m_sections->push_back(new pk_section_t(m__io, this, m__root));
            i++;
        }
    }
}

pbix_t::~pbix_t() {
    _clean_up();
}

void pbix_t::_clean_up() {
    if (m_sections) {
        for (std::vector<pk_section_t*>::iterator it = m_sections->begin(); it != m_sections->end(); ++it) {
            delete *it;
        }
        delete m_sections; m_sections = 0;
    }
}

pbix_t::local_file_t::local_file_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_header = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::local_file_t::_read() {
    m_header = new local_file_header_t(m__io, this, m__root);
    m_body = m__io->read_bytes(header()->len_body_compressed());
}

pbix_t::local_file_t::~local_file_t() {
    _clean_up();
}

void pbix_t::local_file_t::_clean_up() {
    if (m_header) {
        delete m_header; m_header = 0;
    }
}

pbix_t::data_descriptor_t::data_descriptor_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::data_descriptor_t::_read() {
    m_data_descriptor_obs = m__io->read_bytes(12);
}

pbix_t::data_descriptor_t::~data_descriptor_t() {
    _clean_up();
}

void pbix_t::data_descriptor_t::_clean_up() {
}

pbix_t::central_dir_entry_t::central_dir_entry_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::central_dir_entry_t::_read() {
    m_header_obs = m__io->read_bytes(12);
    m_crc32 = m__io->read_u4le();
    m_len_body_compressed = m__io->read_u4le();
    m_len_body_uncompressed = m__io->read_u4le();
    m_len_file_name = m__io->read_u2le();
    m_len_extra = m__io->read_u2le();
    m_len_comment = m__io->read_u2le();
    m_disk_number_start = m__io->read_u2le();
    m_int_file_attr = m__io->read_u2le();
    m_ext_file_attr = m__io->read_u4le();
    m_ofs_local_header = m__io->read_s4le();
    m_file_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(len_file_name()), "UTF-8");
    m_extra = m__io->read_bytes((len_extra() + len_comment()));
}

pbix_t::central_dir_entry_t::~central_dir_entry_t() {
    _clean_up();
}

void pbix_t::central_dir_entry_t::_clean_up() {
}

pbix_t::pk_section_t::pk_section_t(kaitai::kstream* p__io, pbix_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::pk_section_t::_read() {
    m_magic = m__io->read_bytes(2);
    if (!(magic() == std::string("\x50\x4B", 2))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x50\x4B", 2), magic(), _io(), std::string("/types/pk_section/seq/0"));
    }
    m_section_type = m__io->read_u2le();
    n_body = true;
    switch (section_type()) {
    case 513: {
        n_body = false;
        m_body = new central_dir_entry_t(m__io, this, m__root);
        break;
    }
    case 1027: {
        n_body = false;
        m_body = new local_file_t(m__io, this, m__root);
        break;
    }
    case 1541: {
        n_body = false;
        m_body = new end_of_central_dir_t(m__io, this, m__root);
        break;
    }
    case 2055: {
        n_body = false;
        m_body = new data_descriptor_t(m__io, this, m__root);
        break;
    }
    }
}

pbix_t::pk_section_t::~pk_section_t() {
    _clean_up();
}

void pbix_t::pk_section_t::_clean_up() {
    if (!n_body) {
        if (m_body) {
            delete m_body; m_body = 0;
        }
    }
}

pbix_t::local_file_header_t::local_file_header_t(kaitai::kstream* p__io, pbix_t::local_file_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::local_file_header_t::_read() {
    m_header_trimmed = m__io->read_bytes(14);
    m_len_body_compressed = m__io->read_u4le();
    m_len_body_uncompressed = m__io->read_u4le();
    m_len_file_name = m__io->read_u2le();
    m_len_extra = m__io->read_u2le();
    m_file_name = m__io->read_bytes(len_file_name());
    m_extra = m__io->read_bytes(len_extra());
}

pbix_t::local_file_header_t::~local_file_header_t() {
    _clean_up();
}

void pbix_t::local_file_header_t::_clean_up() {
}

pbix_t::end_of_central_dir_t::end_of_central_dir_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::end_of_central_dir_t::_read() {
    m_header_obs = m__io->read_bytes(16);
    m_len_comment = m__io->read_u2le();
    m_comment = m__io->read_bytes(len_comment());
}

pbix_t::end_of_central_dir_t::~end_of_central_dir_t() {
    _clean_up();
}

void pbix_t::end_of_central_dir_t::_clean_up() {
}

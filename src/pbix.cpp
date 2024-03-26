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

pbix_t::chunk_t::chunk_t(kaitai::kstream* p__io, pbix_t::abf_x9_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_node = 0;
    m__io__raw_node = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::chunk_t::_read() {
    m_uncompressed = m__io->read_u4le();
    m_compressed = m__io->read_u4le();
    m__raw_node = m__io->read_bytes(compressed());
    m__io__raw_node = new kaitai::kstream(m__raw_node);
    m_node = new node_t(m__io__raw_node, this, m__root);
}

pbix_t::chunk_t::~chunk_t() {
    _clean_up();
}

void pbix_t::chunk_t::_clean_up() {
    if (m__io__raw_node) {
        delete m__io__raw_node; m__io__raw_node = 0;
    }
    if (m_node) {
        delete m_node; m_node = 0;
    }
}

pbix_t::huffman_flags_t::huffman_flags_t(kaitai::kstream* p__io, pbix_t::header_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::huffman_flags_t::_read() {
    m_huffman_table_length = m__io->read_bits_int_le(13);
    m_log2_window_size = m__io->read_bits_int_le(3);
    m_mtf_entries = static_cast<pbix_t::mtf_entries_enum_t>(m__io->read_bits_int_le(2));
    m_min_ptr_match_length_flag = m__io->read_bits_int_le(1);
    m_min_mtf_match_length_flag = m__io->read_bits_int_le(1);
    m_reserved_zero = m__io->read_bits_int_le(12);
}

pbix_t::huffman_flags_t::~huffman_flags_t() {
    _clean_up();
}

void pbix_t::huffman_flags_t::_clean_up() {
}

pbix_t::local_file_t::local_file_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_header = 0;
    m_datamodel = 0;
    m__io__raw_datamodel = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::local_file_t::_read() {
    m_header = new local_file_header_t(m__io, this, m__root);
    n_body = true;
    if (header()->file_name() != std::string("DataModel")) {
        n_body = false;
        m_body = m__io->read_bytes(header()->len_body_compressed());
    }
    n_datamodel = true;
    if (header()->file_name() == (std::string("DataModel"))) {
        n_datamodel = false;
        m__raw_datamodel = m__io->read_bytes(header()->len_body_compressed());
        m__io__raw_datamodel = new kaitai::kstream(m__raw_datamodel);
        m_datamodel = new abf_x9_t(m__io__raw_datamodel, this, m__root);
    }
}

pbix_t::local_file_t::~local_file_t() {
    _clean_up();
}

void pbix_t::local_file_t::_clean_up() {
    if (m_header) {
        delete m_header; m_header = 0;
    }
    if (!n_body) {
    }
    if (!n_datamodel) {
        if (m__io__raw_datamodel) {
            delete m__io__raw_datamodel; m__io__raw_datamodel = 0;
        }
        if (m_datamodel) {
            delete m_datamodel; m_datamodel = 0;
        }
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
    m_crc32 = m__io->read_u4le();
    m_len_body_compressed = m__io->read_u4le();
    m_len_body_uncompressed = m__io->read_u4le();
}

pbix_t::data_descriptor_t::~data_descriptor_t() {
    _clean_up();
}

void pbix_t::data_descriptor_t::_clean_up() {
}

pbix_t::extra_field_t::extra_field_t(kaitai::kstream* p__io, pbix_t::extras_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__io__raw_body = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extra_field_t::_read() {
    m_code = static_cast<pbix_t::extra_codes_t>(m__io->read_u2le());
    m_len_body = m__io->read_u2le();
    n_body = true;
    switch (code()) {
    case pbix_t::EXTRA_CODES_NTFS: {
        n_body = false;
        m__raw_body = m__io->read_bytes(len_body());
        m__io__raw_body = new kaitai::kstream(m__raw_body);
        m_body = new ntfs_t(m__io__raw_body, this, m__root);
        break;
    }
    case pbix_t::EXTRA_CODES_EXTENDED_TIMESTAMP: {
        n_body = false;
        m__raw_body = m__io->read_bytes(len_body());
        m__io__raw_body = new kaitai::kstream(m__raw_body);
        m_body = new extended_timestamp_t(m__io__raw_body, this, m__root);
        break;
    }
    case pbix_t::EXTRA_CODES_INFOZIP_UNIX_VAR_SIZE: {
        n_body = false;
        m__raw_body = m__io->read_bytes(len_body());
        m__io__raw_body = new kaitai::kstream(m__raw_body);
        m_body = new infozip_unix_var_size_t(m__io__raw_body, this, m__root);
        break;
    }
    default: {
        m__raw_body = m__io->read_bytes(len_body());
        break;
    }
    }
}

pbix_t::extra_field_t::~extra_field_t() {
    _clean_up();
}

void pbix_t::extra_field_t::_clean_up() {
    if (!n_body) {
        if (m__io__raw_body) {
            delete m__io__raw_body; m__io__raw_body = 0;
        }
        if (m_body) {
            delete m_body; m_body = 0;
        }
    }
}

pbix_t::extra_field_t::ntfs_t::ntfs_t(kaitai::kstream* p__io, pbix_t::extra_field_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_attributes = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extra_field_t::ntfs_t::_read() {
    m_reserved = m__io->read_u4le();
    m_attributes = new std::vector<attribute_t*>();
    {
        int i = 0;
        while (!m__io->is_eof()) {
            m_attributes->push_back(new attribute_t(m__io, this, m__root));
            i++;
        }
    }
}

pbix_t::extra_field_t::ntfs_t::~ntfs_t() {
    _clean_up();
}

void pbix_t::extra_field_t::ntfs_t::_clean_up() {
    if (m_attributes) {
        for (std::vector<attribute_t*>::iterator it = m_attributes->begin(); it != m_attributes->end(); ++it) {
            delete *it;
        }
        delete m_attributes; m_attributes = 0;
    }
}

pbix_t::extra_field_t::ntfs_t::attribute_t::attribute_t(kaitai::kstream* p__io, pbix_t::extra_field_t::ntfs_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m__io__raw_body = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extra_field_t::ntfs_t::attribute_t::_read() {
    m_tag = m__io->read_u2le();
    m_len_body = m__io->read_u2le();
    n_body = true;
    switch (tag()) {
    case 1: {
        n_body = false;
        m__raw_body = m__io->read_bytes(len_body());
        m__io__raw_body = new kaitai::kstream(m__raw_body);
        m_body = new attribute_1_t(m__io__raw_body, this, m__root);
        break;
    }
    default: {
        m__raw_body = m__io->read_bytes(len_body());
        break;
    }
    }
}

pbix_t::extra_field_t::ntfs_t::attribute_t::~attribute_t() {
    _clean_up();
}

void pbix_t::extra_field_t::ntfs_t::attribute_t::_clean_up() {
    if (!n_body) {
        if (m__io__raw_body) {
            delete m__io__raw_body; m__io__raw_body = 0;
        }
        if (m_body) {
            delete m_body; m_body = 0;
        }
    }
}

pbix_t::extra_field_t::ntfs_t::attribute_1_t::attribute_1_t(kaitai::kstream* p__io, pbix_t::extra_field_t::ntfs_t::attribute_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extra_field_t::ntfs_t::attribute_1_t::_read() {
    m_last_mod_time = m__io->read_u8le();
    m_last_access_time = m__io->read_u8le();
    m_creation_time = m__io->read_u8le();
}

pbix_t::extra_field_t::ntfs_t::attribute_1_t::~attribute_1_t() {
    _clean_up();
}

void pbix_t::extra_field_t::ntfs_t::attribute_1_t::_clean_up() {
}

pbix_t::extra_field_t::extended_timestamp_t::extended_timestamp_t(kaitai::kstream* p__io, pbix_t::extra_field_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_flags = 0;
    m__io__raw_flags = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extra_field_t::extended_timestamp_t::_read() {
    m__raw_flags = m__io->read_bytes(1);
    m__io__raw_flags = new kaitai::kstream(m__raw_flags);
    m_flags = new info_flags_t(m__io__raw_flags, this, m__root);
    n_mod_time = true;
    if (flags()->has_mod_time()) {
        n_mod_time = false;
        m_mod_time = m__io->read_u4le();
    }
    n_access_time = true;
    if (flags()->has_access_time()) {
        n_access_time = false;
        m_access_time = m__io->read_u4le();
    }
    n_create_time = true;
    if (flags()->has_create_time()) {
        n_create_time = false;
        m_create_time = m__io->read_u4le();
    }
}

pbix_t::extra_field_t::extended_timestamp_t::~extended_timestamp_t() {
    _clean_up();
}

void pbix_t::extra_field_t::extended_timestamp_t::_clean_up() {
    if (m__io__raw_flags) {
        delete m__io__raw_flags; m__io__raw_flags = 0;
    }
    if (m_flags) {
        delete m_flags; m_flags = 0;
    }
    if (!n_mod_time) {
    }
    if (!n_access_time) {
    }
    if (!n_create_time) {
    }
}

pbix_t::extra_field_t::extended_timestamp_t::info_flags_t::info_flags_t(kaitai::kstream* p__io, pbix_t::extra_field_t::extended_timestamp_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extra_field_t::extended_timestamp_t::info_flags_t::_read() {
    m_has_mod_time = m__io->read_bits_int_le(1);
    m_has_access_time = m__io->read_bits_int_le(1);
    m_has_create_time = m__io->read_bits_int_le(1);
    m_reserved = m__io->read_bits_int_le(5);
}

pbix_t::extra_field_t::extended_timestamp_t::info_flags_t::~info_flags_t() {
    _clean_up();
}

void pbix_t::extra_field_t::extended_timestamp_t::info_flags_t::_clean_up() {
}

pbix_t::extra_field_t::infozip_unix_var_size_t::infozip_unix_var_size_t(kaitai::kstream* p__io, pbix_t::extra_field_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extra_field_t::infozip_unix_var_size_t::_read() {
    m_version = m__io->read_u1();
    m_len_uid = m__io->read_u1();
    m_uid = m__io->read_bytes(len_uid());
    m_len_gid = m__io->read_u1();
    m_gid = m__io->read_bytes(len_gid());
}

pbix_t::extra_field_t::infozip_unix_var_size_t::~infozip_unix_var_size_t() {
    _clean_up();
}

void pbix_t::extra_field_t::infozip_unix_var_size_t::_clean_up() {
}

pbix_t::central_dir_entry_t::central_dir_entry_t(kaitai::kstream* p__io, pbix_t::pk_section_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_file_mod_time = 0;
    m__io__raw_file_mod_time = 0;
    m_extra = 0;
    m__io__raw_extra = 0;
    m_local_header = 0;
    f_local_header = false;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::central_dir_entry_t::_read() {
    m_version_made_by = m__io->read_u2le();
    m_version_needed_to_extract = m__io->read_u2le();
    m_flags = m__io->read_u2le();
    m_compression_method = static_cast<pbix_t::compression_t>(m__io->read_u2le());
    m__raw_file_mod_time = m__io->read_bytes(4);
    m__io__raw_file_mod_time = new kaitai::kstream(m__raw_file_mod_time);
    m_file_mod_time = new dos_datetime_t(m__io__raw_file_mod_time);
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
    m__raw_extra = m__io->read_bytes(len_extra());
    m__io__raw_extra = new kaitai::kstream(m__raw_extra);
    m_extra = new extras_t(m__io__raw_extra, this, m__root);
    m_comment = kaitai::kstream::bytes_to_str(m__io->read_bytes(len_comment()), "UTF-8");
}

pbix_t::central_dir_entry_t::~central_dir_entry_t() {
    _clean_up();
}

void pbix_t::central_dir_entry_t::_clean_up() {
    if (m__io__raw_file_mod_time) {
        delete m__io__raw_file_mod_time; m__io__raw_file_mod_time = 0;
    }
    if (m_file_mod_time) {
        delete m_file_mod_time; m_file_mod_time = 0;
    }
    if (m__io__raw_extra) {
        delete m__io__raw_extra; m__io__raw_extra = 0;
    }
    if (m_extra) {
        delete m_extra; m_extra = 0;
    }
    if (f_local_header) {
        if (m_local_header) {
            delete m_local_header; m_local_header = 0;
        }
    }
}

pbix_t::pk_section_t* pbix_t::central_dir_entry_t::local_header() {
    if (f_local_header)
        return m_local_header;
    std::streampos _pos = m__io->pos();
    m__io->seek(ofs_local_header());
    m_local_header = new pk_section_t(m__io, this, m__root);
    m__io->seek(_pos);
    f_local_header = true;
    return m_local_header;
}

pbix_t::pk_section_t::pk_section_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
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

pbix_t::abf_x9_t::abf_x9_t(kaitai::kstream* p__io, pbix_t::local_file_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_chunks = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::abf_x9_t::_read() {
    m_signature = kaitai::kstream::bytes_to_str(m__io->read_bytes(102), "UTF-16LE");
    m_chunks = new std::vector<chunk_t*>();
    {
        int i = 0;
        while (!m__io->is_eof()) {
            m_chunks->push_back(new chunk_t(m__io, this, m__root));
            i++;
        }
    }
}

pbix_t::abf_x9_t::~abf_x9_t() {
    _clean_up();
}

void pbix_t::abf_x9_t::_clean_up() {
    if (m_chunks) {
        for (std::vector<chunk_t*>::iterator it = m_chunks->begin(); it != m_chunks->end(); ++it) {
            delete *it;
        }
        delete m_chunks; m_chunks = 0;
    }
}

pbix_t::extras_t::extras_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_entries = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::extras_t::_read() {
    m_entries = new std::vector<extra_field_t*>();
    {
        int i = 0;
        while (!m__io->is_eof()) {
            m_entries->push_back(new extra_field_t(m__io, this, m__root));
            i++;
        }
    }
}

pbix_t::extras_t::~extras_t() {
    _clean_up();
}

void pbix_t::extras_t::_clean_up() {
    if (m_entries) {
        for (std::vector<extra_field_t*>::iterator it = m_entries->begin(); it != m_entries->end(); ++it) {
            delete *it;
        }
        delete m_entries; m_entries = 0;
    }
}

pbix_t::local_file_header_t::local_file_header_t(kaitai::kstream* p__io, pbix_t::local_file_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_flags = 0;
    m__io__raw_flags = 0;
    m_file_mod_time = 0;
    m__io__raw_file_mod_time = 0;
    m_extra = 0;
    m__io__raw_extra = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::local_file_header_t::_read() {
    m_version = m__io->read_u2le();
    m__raw_flags = m__io->read_bytes(2);
    m__io__raw_flags = new kaitai::kstream(m__raw_flags);
    m_flags = new gp_flags_t(m__io__raw_flags, this, m__root);
    m_compression_method = static_cast<pbix_t::compression_t>(m__io->read_u2le());
    m__raw_file_mod_time = m__io->read_bytes(4);
    m__io__raw_file_mod_time = new kaitai::kstream(m__raw_file_mod_time);
    m_file_mod_time = new dos_datetime_t(m__io__raw_file_mod_time);
    m_crc32 = m__io->read_u4le();
    m_len_body_compressed = m__io->read_u4le();
    m_len_body_uncompressed = m__io->read_u4le();
    m_len_file_name = m__io->read_u2le();
    m_len_extra = m__io->read_u2le();
    m_file_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(len_file_name()), "UTF-8");
    m__raw_extra = m__io->read_bytes(len_extra());
    m__io__raw_extra = new kaitai::kstream(m__raw_extra);
    m_extra = new extras_t(m__io__raw_extra, this, m__root);
}

pbix_t::local_file_header_t::~local_file_header_t() {
    _clean_up();
}

void pbix_t::local_file_header_t::_clean_up() {
    if (m__io__raw_flags) {
        delete m__io__raw_flags; m__io__raw_flags = 0;
    }
    if (m_flags) {
        delete m_flags; m_flags = 0;
    }
    if (m__io__raw_file_mod_time) {
        delete m__io__raw_file_mod_time; m__io__raw_file_mod_time = 0;
    }
    if (m_file_mod_time) {
        delete m_file_mod_time; m_file_mod_time = 0;
    }
    if (m__io__raw_extra) {
        delete m__io__raw_extra; m__io__raw_extra = 0;
    }
    if (m_extra) {
        delete m_extra; m_extra = 0;
    }
}

pbix_t::local_file_header_t::gp_flags_t::gp_flags_t(kaitai::kstream* p__io, pbix_t::local_file_header_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_deflated_mode = false;
    f_imploded_dict_byte_size = false;
    f_imploded_num_sf_trees = false;
    f_lzma_has_eos_marker = false;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::local_file_header_t::gp_flags_t::_read() {
    m_file_encrypted = m__io->read_bits_int_le(1);
    m_comp_options_raw = m__io->read_bits_int_le(2);
    m_has_data_descriptor = m__io->read_bits_int_le(1);
    m_reserved_1 = m__io->read_bits_int_le(1);
    m_comp_patched_data = m__io->read_bits_int_le(1);
    m_strong_encrypt = m__io->read_bits_int_le(1);
    m_reserved_2 = m__io->read_bits_int_le(4);
    m_lang_encoding = m__io->read_bits_int_le(1);
    m_reserved_3 = m__io->read_bits_int_le(1);
    m_mask_header_values = m__io->read_bits_int_le(1);
    m_reserved_4 = m__io->read_bits_int_le(2);
}

pbix_t::local_file_header_t::gp_flags_t::~gp_flags_t() {
    _clean_up();
}

void pbix_t::local_file_header_t::gp_flags_t::_clean_up() {
}

pbix_t::local_file_header_t::gp_flags_t::deflate_mode_t pbix_t::local_file_header_t::gp_flags_t::deflated_mode() {
    if (f_deflated_mode)
        return m_deflated_mode;
    n_deflated_mode = true;
    if ( ((_parent()->compression_method() == pbix_t::COMPRESSION_DEFLATED) || (_parent()->compression_method() == pbix_t::COMPRESSION_ENHANCED_DEFLATED)) ) {
        n_deflated_mode = false;
        m_deflated_mode = static_cast<pbix_t::local_file_header_t::gp_flags_t::deflate_mode_t>(comp_options_raw());
    }
    f_deflated_mode = true;
    return m_deflated_mode;
}

int32_t pbix_t::local_file_header_t::gp_flags_t::imploded_dict_byte_size() {
    if (f_imploded_dict_byte_size)
        return m_imploded_dict_byte_size;
    n_imploded_dict_byte_size = true;
    if (_parent()->compression_method() == pbix_t::COMPRESSION_IMPLODED) {
        n_imploded_dict_byte_size = false;
        m_imploded_dict_byte_size = ((((comp_options_raw() & 1) != 0) ? (8) : (4)) * 1024);
    }
    f_imploded_dict_byte_size = true;
    return m_imploded_dict_byte_size;
}

int8_t pbix_t::local_file_header_t::gp_flags_t::imploded_num_sf_trees() {
    if (f_imploded_num_sf_trees)
        return m_imploded_num_sf_trees;
    n_imploded_num_sf_trees = true;
    if (_parent()->compression_method() == pbix_t::COMPRESSION_IMPLODED) {
        n_imploded_num_sf_trees = false;
        m_imploded_num_sf_trees = (((comp_options_raw() & 2) != 0) ? (3) : (2));
    }
    f_imploded_num_sf_trees = true;
    return m_imploded_num_sf_trees;
}

bool pbix_t::local_file_header_t::gp_flags_t::lzma_has_eos_marker() {
    if (f_lzma_has_eos_marker)
        return m_lzma_has_eos_marker;
    n_lzma_has_eos_marker = true;
    if (_parent()->compression_method() == pbix_t::COMPRESSION_LZMA) {
        n_lzma_has_eos_marker = false;
        m_lzma_has_eos_marker = (comp_options_raw() & 1) != 0;
    }
    f_lzma_has_eos_marker = true;
    return m_lzma_has_eos_marker;
}

pbix_t::header_t::header_t(kaitai::kstream* p__io, pbix_t::node_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_huffman_table_flags = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void pbix_t::header_t::_read() {
    m_xpress_magic = m__io->read_bytes(4);
    if (!(xpress_magic() == std::string("\x2A\xD7\x86\x4E", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x2A\xD7\x86\x4E", 4), xpress_magic(), _io(), std::string("/types/header/seq/0"));
    }
    m_orig_size = m__io->read_u4le();
    m_encoded_size = m__io->read_u4le();
    m_huffman_table_flags = new huffman_flags_t(m__io, this, m__root);
    m_zero = m__io->read_u4le();
    m_session_signature = m__io->read_u4le();
    m_block_index = m__io->read_u4le();
    m_crc32 = m__io->read_u4le();
}

pbix_t::header_t::~header_t() {
    _clean_up();
}

void pbix_t::header_t::_clean_up() {
    if (m_huffman_table_flags) {
        delete m_huffman_table_flags; m_huffman_table_flags = 0;
    }
}

pbix_t::node_t::node_t(kaitai::kstream* p__io, pbix_t::chunk_t* p__parent, pbix_t* p__root) : kaitai::kstruct(p__io) {
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

void pbix_t::node_t::_read() {
    m_header = new header_t(m__io, this, m__root);
    m_segments = m__io->read_bytes((_parent()->compressed() - 32));
}

pbix_t::node_t::~node_t() {
    _clean_up();
}

void pbix_t::node_t::_clean_up() {
    if (m_header) {
        delete m_header; m_header = 0;
    }
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
    m_disk_of_end_of_central_dir = m__io->read_u2le();
    m_disk_of_central_dir = m__io->read_u2le();
    m_num_central_dir_entries_on_disk = m__io->read_u2le();
    m_num_central_dir_entries_total = m__io->read_u2le();
    m_len_central_dir = m__io->read_u4le();
    m_ofs_central_dir = m__io->read_u4le();
    m_len_comment = m__io->read_u2le();
    m_comment = kaitai::kstream::bytes_to_str(m__io->read_bytes(len_comment()), "UTF-8");
}

pbix_t::end_of_central_dir_t::~end_of_central_dir_t() {
    _clean_up();
}

void pbix_t::end_of_central_dir_t::_clean_up() {
}

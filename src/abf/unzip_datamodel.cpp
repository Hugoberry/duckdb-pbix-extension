// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "unzip_datamodel.h"
#include "kaitai/exceptions.h"

unzip_datamodel_t::unzip_datamodel_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, unzip_datamodel_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_end_of_central_dir = 0;
    m_central_dir = 0;
    f_end_of_central_dir = false;
    f_central_dir = false;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void unzip_datamodel_t::_read() {
}

unzip_datamodel_t::~unzip_datamodel_t() {
    _clean_up();
}

void unzip_datamodel_t::_clean_up() {
    if (f_end_of_central_dir) {
        if (m_end_of_central_dir) {
            delete m_end_of_central_dir; m_end_of_central_dir = 0;
        }
    }
    if (f_central_dir) {
        if (m_central_dir) {
            for (std::vector<central_dir_entry_t*>::iterator it = m_central_dir->begin(); it != m_central_dir->end(); ++it) {
                delete *it;
            }
            delete m_central_dir; m_central_dir = 0;
        }
    }
}

unzip_datamodel_t::end_of_central_dir_t::end_of_central_dir_t(kaitai::kstream* p__io, unzip_datamodel_t* p__parent, unzip_datamodel_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void unzip_datamodel_t::end_of_central_dir_t::_read() {
    m_eocd_magic = m__io->read_bytes(4);
    if (!(eocd_magic() == std::string("\x50\x4B\x05\x06", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x50\x4B\x05\x06", 4), eocd_magic(), _io(), std::string("/types/end_of_central_dir/seq/0"));
    }
    m_disk_of_end_of_central_dir = m__io->read_u2le();
    m_disk_of_central_dir = m__io->read_u2le();
    m_num_central_dir_entries_on_disk = m__io->read_u2le();
    m_num_central_dir_entries_total = m__io->read_u2le();
    m_len_central_dir = m__io->read_u4le();
    m_ofs_central_dir = m__io->read_u4le();
    m_len_comment = m__io->read_u2le();
}

unzip_datamodel_t::end_of_central_dir_t::~end_of_central_dir_t() {
    _clean_up();
}

void unzip_datamodel_t::end_of_central_dir_t::_clean_up() {
}

unzip_datamodel_t::central_dir_entry_t::central_dir_entry_t(kaitai::kstream* p__io, unzip_datamodel_t* p__parent, unzip_datamodel_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_local_header = 0;
    f_local_header = false;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void unzip_datamodel_t::central_dir_entry_t::_read() {
    m_cd_magic = m__io->read_bytes(4);
    if (!(cd_magic() == std::string("\x50\x4B\x01\x02", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x50\x4B\x01\x02", 4), cd_magic(), _io(), std::string("/types/central_dir_entry/seq/0"));
    }
    m_version_made_by = m__io->read_u2le();
    m_version_needed_to_extract = m__io->read_u2le();
    m_flags = m__io->read_u2le();
    m_compression_method = m__io->read_u2le();
    m_file_mod_time = m__io->read_bytes(4);
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
    m_extra = m__io->read_bytes(len_extra());
    m_comment = kaitai::kstream::bytes_to_str(m__io->read_bytes(len_comment()), "UTF-8");
}

unzip_datamodel_t::central_dir_entry_t::~central_dir_entry_t() {
    _clean_up();
}

void unzip_datamodel_t::central_dir_entry_t::_clean_up() {
    if (f_local_header) {
        if (m_local_header) {
            delete m_local_header; m_local_header = 0;
        }
    }
}

unzip_datamodel_t::local_file_header_t* unzip_datamodel_t::central_dir_entry_t::local_header() {
    if (f_local_header)
        return m_local_header;
    std::streampos _pos = m__io->pos();
    m__io->seek(ofs_local_header());
    m_local_header = new local_file_header_t(m__io, this, m__root);
    m__io->seek(_pos);
    f_local_header = true;
    return m_local_header;
}

unzip_datamodel_t::local_file_header_t::local_file_header_t(kaitai::kstream* p__io, unzip_datamodel_t::central_dir_entry_t* p__parent, unzip_datamodel_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void unzip_datamodel_t::local_file_header_t::_read() {
    m_lfh_magic = m__io->read_bytes(4);
    if (!(lfh_magic() == std::string("\x50\x4B\x03\x04", 4))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x50\x4B\x03\x04", 4), lfh_magic(), _io(), std::string("/types/local_file_header/seq/0"));
    }
    m_version = m__io->read_u2le();
    m_flags = m__io->read_bytes(2);
    m_compression_method = m__io->read_u2le();
    m_file_mod_time = m__io->read_bytes(4);
    m_crc32 = m__io->read_u4le();
    m_len_body_compressed = m__io->read_u4le();
    m_len_body_uncompressed = m__io->read_u4le();
    m_len_file_name = m__io->read_u2le();
    m_len_extra = m__io->read_u2le();
    m_file_name = kaitai::kstream::bytes_to_str(m__io->read_bytes(len_file_name()), "UTF-8");
    m_extra = m__io->read_bytes(len_extra());
}

unzip_datamodel_t::local_file_header_t::~local_file_header_t() {
    _clean_up();
}

void unzip_datamodel_t::local_file_header_t::_clean_up() {
}

unzip_datamodel_t::end_of_central_dir_t* unzip_datamodel_t::end_of_central_dir() {
    if (f_end_of_central_dir)
        return m_end_of_central_dir;
    std::streampos _pos = m__io->pos();
    m__io->seek((_root()->_io()->size() - 22));
    m_end_of_central_dir = new end_of_central_dir_t(m__io, this, m__root);
    m__io->seek(_pos);
    f_end_of_central_dir = true;
    return m_end_of_central_dir;
}

std::vector<unzip_datamodel_t::central_dir_entry_t*>* unzip_datamodel_t::central_dir() {
    if (f_central_dir)
        return m_central_dir;
    std::streampos _pos = m__io->pos();
    m__io->seek(end_of_central_dir()->ofs_central_dir());
    m_central_dir = new std::vector<central_dir_entry_t*>();
    {
        int i = 0;
        central_dir_entry_t* _;
        do {
            _ = new central_dir_entry_t(m__io, this, m__root);
            m_central_dir->push_back(_);
            i++;
        } while (!(_->file_name() == (std::string("DataModel"))));
    }
    m__io->seek(_pos);
    f_central_dir = true;
    return m_central_dir;
}

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "abf_xpress9.h"
#include "kaitai/exceptions.h"

abf_xpress9_t::abf_xpress9_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, abf_xpress9_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_chunks = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void abf_xpress9_t::_read() {
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

abf_xpress9_t::~abf_xpress9_t() {
    _clean_up();
}

void abf_xpress9_t::_clean_up() {
    if (m_chunks) {
        for (std::vector<chunk_t*>::iterator it = m_chunks->begin(); it != m_chunks->end(); ++it) {
            delete *it;
        }
        delete m_chunks; m_chunks = 0;
    }
}

abf_xpress9_t::chunk_t::chunk_t(kaitai::kstream* p__io, abf_xpress9_t* p__parent, abf_xpress9_t* p__root) : kaitai::kstruct(p__io) {
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

void abf_xpress9_t::chunk_t::_read() {
    m_uncompressed = m__io->read_u4le();
    m_compressed = m__io->read_u4le();
    m__raw_node = m__io->read_bytes(compressed());
    m__io__raw_node = new kaitai::kstream(m__raw_node);
    m_node = new node_t(m__io__raw_node, this, m__root);
}

abf_xpress9_t::chunk_t::~chunk_t() {
    _clean_up();
}

void abf_xpress9_t::chunk_t::_clean_up() {
    if (m__io__raw_node) {
        delete m__io__raw_node; m__io__raw_node = 0;
    }
    if (m_node) {
        delete m_node; m_node = 0;
    }
}

abf_xpress9_t::node_t::node_t(kaitai::kstream* p__io, abf_xpress9_t::chunk_t* p__parent, abf_xpress9_t* p__root) : kaitai::kstruct(p__io) {
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

void abf_xpress9_t::node_t::_read() {
    m_header = new header_t(m__io, this, m__root);
    m_segments = m__io->read_bytes((_parent()->compressed() - 32));
}

abf_xpress9_t::node_t::~node_t() {
    _clean_up();
}

void abf_xpress9_t::node_t::_clean_up() {
    if (m_header) {
        delete m_header; m_header = 0;
    }
}

abf_xpress9_t::header_t::header_t(kaitai::kstream* p__io, abf_xpress9_t::node_t* p__parent, abf_xpress9_t* p__root) : kaitai::kstruct(p__io) {
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

void abf_xpress9_t::header_t::_read() {
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

abf_xpress9_t::header_t::~header_t() {
    _clean_up();
}

void abf_xpress9_t::header_t::_clean_up() {
    if (m_huffman_table_flags) {
        delete m_huffman_table_flags; m_huffman_table_flags = 0;
    }
}

abf_xpress9_t::huffman_flags_t::huffman_flags_t(kaitai::kstream* p__io, abf_xpress9_t::header_t* p__parent, abf_xpress9_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void abf_xpress9_t::huffman_flags_t::_read() {
    m_huffman_table_length = m__io->read_bits_int_be(13);
    m_log2_window_size = m__io->read_bits_int_be(3);
    m_mtf_entries = static_cast<abf_xpress9_t::mtf_entries_enum_t>(m__io->read_bits_int_be(2));
    m_min_ptr_match_length_flag = m__io->read_bits_int_be(1);
    m_min_mtf_match_length_flag = m__io->read_bits_int_be(1);
    m_reserved_zero = m__io->read_bits_int_be(12);
}

abf_xpress9_t::huffman_flags_t::~huffman_flags_t() {
    _clean_up();
}

void abf_xpress9_t::huffman_flags_t::_clean_up() {
}

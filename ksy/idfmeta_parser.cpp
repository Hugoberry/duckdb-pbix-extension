// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "idfmeta_parser.h"
#include "kaitai/exceptions.h"

idfmeta_parser_t::idfmeta_parser_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_blocks = 0;
    f_bit_width = false;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::_read() {
    m_blocks = new block_t(m__io, this, m__root);
}

idfmeta_parser_t::~idfmeta_parser_t() {
    _clean_up();
}

void idfmeta_parser_t::_clean_up() {
    if (m_blocks) {
        delete m_blocks; m_blocks = 0;
    }
}

idfmeta_parser_t::c_p_element_t::c_p_element_t(kaitai::kstream* p__io, idfmeta_parser_t::block_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_cs = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::c_p_element_t::_read() {
    m_cp_tag = m__io->read_bytes(6);
    if (!(cp_tag() == std::string("\x3C\x31\x3A\x43\x50\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x3C\x31\x3A\x43\x50\x00", 6), cp_tag(), _io(), std::string("/types/c_p_element/seq/0"));
    }
    m_version_one = m__io->read_u8le();
    m_cs = new c_s0_element_t(m__io, this, m__root);
    m_cp_tag_end = m__io->read_bytes(6);
    if (!(cp_tag_end() == std::string("\x43\x50\x3A\x31\x3E\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x43\x50\x3A\x31\x3E\x00", 6), cp_tag_end(), _io(), std::string("/types/c_p_element/seq/3"));
    }
}

idfmeta_parser_t::c_p_element_t::~c_p_element_t() {
    _clean_up();
}

void idfmeta_parser_t::c_p_element_t::_clean_up() {
    if (m_cs) {
        delete m_cs; m_cs = 0;
    }
}

idfmeta_parser_t::c_s1_element_t::c_s1_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_s0_element_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::c_s1_element_t::_read() {
    m_cs_tag = m__io->read_bytes(6);
    if (!(cs_tag() == std::string("\x3C\x31\x3A\x43\x53\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x3C\x31\x3A\x43\x53\x00", 6), cs_tag(), _io(), std::string("/types/c_s1_element/seq/0"));
    }
    m_count_bit_packed = m__io->read_u8le();
    m_blob_with9_zeros = m__io->read_bytes(9);
    m_cs_tag_end = m__io->read_bytes(6);
    if (!(cs_tag_end() == std::string("\x43\x53\x3A\x31\x3E\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x43\x53\x3A\x31\x3E\x00", 6), cs_tag_end(), _io(), std::string("/types/c_s1_element/seq/3"));
    }
}

idfmeta_parser_t::c_s1_element_t::~c_s1_element_t() {
    _clean_up();
}

void idfmeta_parser_t::c_s1_element_t::_clean_up() {
}

idfmeta_parser_t::c_s0_element_t::c_s0_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_p_element_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_ss = 0;
    m_cs = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::c_s0_element_t::_read() {
    m_cs_tag = m__io->read_bytes(6);
    if (!(cs_tag() == std::string("\x3C\x31\x3A\x43\x53\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x3C\x31\x3A\x43\x53\x00", 6), cs_tag(), _io(), std::string("/types/c_s0_element/seq/0"));
    }
    m_records = m__io->read_u8le();
    m_one = m__io->read_u8le();
    m_a_b_a_5_a = m__io->read_u4le();
    m_iterator = m__io->read_u4le();
    m_bookmark_bits_1_2_8 = m__io->read_u8le();
    m_storage_alloc_size = m__io->read_u8le();
    m_storage_used_size = m__io->read_u8le();
    m_segment_needs_resizing = m__io->read_u1();
    m_compression_info = m__io->read_u4le();
    m_ss = new s_s_element_t(m__io, this, m__root);
    m_has_bit_packed_sub_seg = m__io->read_u1();
    m_cs = new c_s1_element_t(m__io, this, m__root);
    m_cs_tag_end = m__io->read_bytes(6);
    if (!(cs_tag_end() == std::string("\x43\x53\x3A\x31\x3E\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x43\x53\x3A\x31\x3E\x00", 6), cs_tag_end(), _io(), std::string("/types/c_s0_element/seq/13"));
    }
}

idfmeta_parser_t::c_s0_element_t::~c_s0_element_t() {
    _clean_up();
}

void idfmeta_parser_t::c_s0_element_t::_clean_up() {
    if (m_ss) {
        delete m_ss; m_ss = 0;
    }
    if (m_cs) {
        delete m_cs; m_cs = 0;
    }
}

idfmeta_parser_t::s_d_os_element_t::s_d_os_element_t(kaitai::kstream* p__io, idfmeta_parser_t::block_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_csdos = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::s_d_os_element_t::_read() {
    m_sdos_tag = m__io->read_bytes(8);
    if (!(sdos_tag() == std::string("\x3C\x31\x3A\x53\x44\x4F\x73\x00", 8))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x3C\x31\x3A\x53\x44\x4F\x73\x00", 8), sdos_tag(), _io(), std::string("/types/s_d_os_element/seq/0"));
    }
    m_csdos = new c_s_d_os_element_t(m__io, this, m__root);
    m_sdos_tag_end = m__io->read_bytes(8);
    if (!(sdos_tag_end() == std::string("\x53\x44\x4F\x73\x3A\x31\x3E\x00", 8))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x53\x44\x4F\x73\x3A\x31\x3E\x00", 8), sdos_tag_end(), _io(), std::string("/types/s_d_os_element/seq/2"));
    }
}

idfmeta_parser_t::s_d_os_element_t::~s_d_os_element_t() {
    _clean_up();
}

void idfmeta_parser_t::s_d_os_element_t::_clean_up() {
    if (m_csdos) {
        delete m_csdos; m_csdos = 0;
    }
}

idfmeta_parser_t::s_s_element_t::s_s_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_s0_element_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::s_s_element_t::_read() {
    m_ss_tag = m__io->read_bytes(6);
    if (!(ss_tag() == std::string("\x3C\x31\x3A\x53\x53\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x3C\x31\x3A\x53\x53\x00", 6), ss_tag(), _io(), std::string("/types/s_s_element/seq/0"));
    }
    m_distinct_states = m__io->read_u8le();
    m_min_data_id = m__io->read_u4le();
    m_max_data_id = m__io->read_u4le();
    m_original_min_segment_data_id = m__io->read_u4le();
    m_r_l_e_sort_order = m__io->read_s8le();
    m_row_count = m__io->read_u8le();
    m_has_nulls = m__io->read_u1();
    m_r_l_e_runs = m__io->read_u8le();
    m_others_r_l_e_runs = m__io->read_u8le();
    m_ss_tag_end = m__io->read_bytes(6);
    if (!(ss_tag_end() == std::string("\x53\x53\x3A\x31\x3E\x00", 6))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x53\x53\x3A\x31\x3E\x00", 6), ss_tag_end(), _io(), std::string("/types/s_s_element/seq/10"));
    }
}

idfmeta_parser_t::s_s_element_t::~s_s_element_t() {
    _clean_up();
}

void idfmeta_parser_t::s_s_element_t::_clean_up() {
}

idfmeta_parser_t::block_t::block_t(kaitai::kstream* p__io, idfmeta_parser_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_cp = 0;
    m_sdos = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::block_t::_read() {
    m_cp = new c_p_element_t(m__io, this, m__root);
    m_sdos = new s_d_os_element_t(m__io, this, m__root);
}

idfmeta_parser_t::block_t::~block_t() {
    _clean_up();
}

void idfmeta_parser_t::block_t::_clean_up() {
    if (m_cp) {
        delete m_cp; m_cp = 0;
    }
    if (m_sdos) {
        delete m_sdos; m_sdos = 0;
    }
}

idfmeta_parser_t::c_s_d_os1_element_t::c_s_d_os1_element_t(kaitai::kstream* p__io, idfmeta_parser_t::c_s_d_os_element_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::c_s_d_os1_element_t::_read() {
    m_csdos_tag = m__io->read_bytes(9);
    if (!(csdos_tag() == std::string("\x3C\x31\x3A\x43\x53\x44\x4F\x73\x00", 9))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x3C\x31\x3A\x43\x53\x44\x4F\x73\x00", 9), csdos_tag(), _io(), std::string("/types/c_s_d_os1_element/seq/0"));
    }
    m_sub_segment_offset = m__io->read_u8le();
    m_sub_segment_size = m__io->read_u8le();
    m_csdos_tag_end = m__io->read_bytes(9);
    if (!(csdos_tag_end() == std::string("\x43\x53\x44\x4F\x73\x3A\x31\x3E\x00", 9))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x43\x53\x44\x4F\x73\x3A\x31\x3E\x00", 9), csdos_tag_end(), _io(), std::string("/types/c_s_d_os1_element/seq/3"));
    }
}

idfmeta_parser_t::c_s_d_os1_element_t::~c_s_d_os1_element_t() {
    _clean_up();
}

void idfmeta_parser_t::c_s_d_os1_element_t::_clean_up() {
}

idfmeta_parser_t::c_s_d_os_element_t::c_s_d_os_element_t(kaitai::kstream* p__io, idfmeta_parser_t::s_d_os_element_t* p__parent, idfmeta_parser_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    m_csdos = 0;

    try {
        _read();
    } catch(...) {
        _clean_up();
        throw;
    }
}

void idfmeta_parser_t::c_s_d_os_element_t::_read() {
    m_csdos_tag = m__io->read_bytes(9);
    if (!(csdos_tag() == std::string("\x3C\x31\x3A\x43\x53\x44\x4F\x73\x00", 9))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x3C\x31\x3A\x43\x53\x44\x4F\x73\x00", 9), csdos_tag(), _io(), std::string("/types/c_s_d_os_element/seq/0"));
    }
    m_zero_c_s_d_o = m__io->read_u8le();
    m_primary_segment_size = m__io->read_u8le();
    m_csdos = new c_s_d_os1_element_t(m__io, this, m__root);
    m_csdos_tag_end = m__io->read_bytes(9);
    if (!(csdos_tag_end() == std::string("\x43\x53\x44\x4F\x73\x3A\x31\x3E\x00", 9))) {
        throw kaitai::validation_not_equal_error<std::string>(std::string("\x43\x53\x44\x4F\x73\x3A\x31\x3E\x00", 9), csdos_tag_end(), _io(), std::string("/types/c_s_d_os_element/seq/4"));
    }
}

idfmeta_parser_t::c_s_d_os_element_t::~c_s_d_os_element_t() {
    _clean_up();
}

void idfmeta_parser_t::c_s_d_os_element_t::_clean_up() {
    if (m_csdos) {
        delete m_csdos; m_csdos = 0;
    }
}

int32_t idfmeta_parser_t::bit_width() {
    if (f_bit_width)
        return m_bit_width;
    m_bit_width = ((36 - blocks()->cp()->cs()->a_b_a_5_a()) + blocks()->cp()->cs()->iterator());
    f_bit_width = true;
    return m_bit_width;
}

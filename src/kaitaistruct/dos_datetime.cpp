// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "dos_datetime.h"
#include "kaitai/exceptions.h"

dos_datetime_t::dos_datetime_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent, dos_datetime_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = this;
    m_time = nullptr;
    m_date = nullptr;
    _read();
}

void dos_datetime_t::_read() {
    m_time = std::unique_ptr<time_t>(new time_t(m__io, this, m__root));
    m_date = std::unique_ptr<date_t>(new date_t(m__io, this, m__root));
}

dos_datetime_t::~dos_datetime_t() {
    _clean_up();
}

void dos_datetime_t::_clean_up() {
}

dos_datetime_t::time_t::time_t(kaitai::kstream* p__io, dos_datetime_t* p__parent, dos_datetime_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_second = false;
    f_padded_second = false;
    f_padded_minute = false;
    f_padded_hour = false;
    _read();
}

void dos_datetime_t::time_t::_read() {
    m_second_div_2 = m__io->read_bits_int_le(5);
    if (!(second_div_2() <= 29)) {
        throw kaitai::validation_greater_than_error<uint64_t>(29, second_div_2(), _io(), std::string("/types/time/seq/0"));
    }
    m_minute = m__io->read_bits_int_le(6);
    if (!(minute() <= 59)) {
        throw kaitai::validation_greater_than_error<uint64_t>(59, minute(), _io(), std::string("/types/time/seq/1"));
    }
    m_hour = m__io->read_bits_int_le(5);
    if (!(hour() <= 23)) {
        throw kaitai::validation_greater_than_error<uint64_t>(23, hour(), _io(), std::string("/types/time/seq/2"));
    }
}

dos_datetime_t::time_t::~time_t() {
    _clean_up();
}

void dos_datetime_t::time_t::_clean_up() {
}

int32_t dos_datetime_t::time_t::second() {
    if (f_second)
        return m_second;
    m_second = (2 * second_div_2());
    f_second = true;
    return m_second;
}

std::string dos_datetime_t::time_t::padded_second() {
    if (f_padded_second)
        return m_padded_second;
    m_padded_second = ((second() <= 9) ? (std::string("0")) : (std::string(""))) + kaitai::kstream::to_string(second());
    f_padded_second = true;
    return m_padded_second;
}

std::string dos_datetime_t::time_t::padded_minute() {
    if (f_padded_minute)
        return m_padded_minute;
    m_padded_minute = ((minute() <= 9) ? (std::string("0")) : (std::string(""))) + kaitai::kstream::to_string(minute());
    f_padded_minute = true;
    return m_padded_minute;
}

std::string dos_datetime_t::time_t::padded_hour() {
    if (f_padded_hour)
        return m_padded_hour;
    m_padded_hour = ((hour() <= 9) ? (std::string("0")) : (std::string(""))) + kaitai::kstream::to_string(hour());
    f_padded_hour = true;
    return m_padded_hour;
}

dos_datetime_t::date_t::date_t(kaitai::kstream* p__io, dos_datetime_t* p__parent, dos_datetime_t* p__root) : kaitai::kstruct(p__io) {
    m__parent = p__parent;
    m__root = p__root;
    f_year = false;
    f_padded_day = false;
    f_padded_month = false;
    f_padded_year = false;
    _read();
}

void dos_datetime_t::date_t::_read() {
    m_day = m__io->read_bits_int_le(5);
    if (!(day() >= 1)) {
        throw kaitai::validation_less_than_error<uint64_t>(1, day(), _io(), std::string("/types/date/seq/0"));
    }
    m_month = m__io->read_bits_int_le(4);
    if (!(month() >= 1)) {
        throw kaitai::validation_less_than_error<uint64_t>(1, month(), _io(), std::string("/types/date/seq/1"));
    }
    if (!(month() <= 12)) {
        throw kaitai::validation_greater_than_error<uint64_t>(12, month(), _io(), std::string("/types/date/seq/1"));
    }
    m_year_minus_1980 = m__io->read_bits_int_le(7);
}

dos_datetime_t::date_t::~date_t() {
    _clean_up();
}

void dos_datetime_t::date_t::_clean_up() {
}

int32_t dos_datetime_t::date_t::year() {
    if (f_year)
        return m_year;
    m_year = (1980 + year_minus_1980());
    f_year = true;
    return m_year;
}

std::string dos_datetime_t::date_t::padded_day() {
    if (f_padded_day)
        return m_padded_day;
    m_padded_day = ((day() <= 9) ? (std::string("0")) : (std::string(""))) + kaitai::kstream::to_string(day());
    f_padded_day = true;
    return m_padded_day;
}

std::string dos_datetime_t::date_t::padded_month() {
    if (f_padded_month)
        return m_padded_month;
    m_padded_month = ((month() <= 9) ? (std::string("0")) : (std::string(""))) + kaitai::kstream::to_string(month());
    f_padded_month = true;
    return m_padded_month;
}

std::string dos_datetime_t::date_t::padded_year() {
    if (f_padded_year)
        return m_padded_year;
    m_padded_year = ((year() <= 999) ? (std::string("0") + ((year() <= 99) ? (std::string("0") + ((year() <= 9) ? (std::string("0")) : (std::string("")))) : (std::string("")))) : (std::string(""))) + kaitai::kstream::to_string(year());
    f_padded_year = true;
    return m_padded_year;
}

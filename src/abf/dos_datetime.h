#ifndef DOS_DATETIME_H_
#define DOS_DATETIME_H_

// This is a generated file! Please edit source .ksy file and use kaitai-struct-compiler to rebuild

#include "../kaitai/kaitaistruct.h"
#include <stdint.h>

#if KAITAI_STRUCT_VERSION < 9000L
#error "Incompatible Kaitai Struct C++/STL API: version 0.9 or later is required"
#endif

/**
 * MS-DOS date and time are packed 16-bit values that specify local date/time.
 * The time is always stored in the current UTC time offset set on the computer
 * which created the file. Note that the daylight saving time (DST) shifts
 * also change the UTC time offset.
 * 
 * For example, if you pack two files A and B into a ZIP archive, file A last modified
 * at 2020-03-29 00:59 UTC+00:00 (GMT) and file B at 2020-03-29 02:00 UTC+01:00 (BST),
 * the file modification times saved in MS-DOS format in the ZIP file will vary depending
 * on whether the computer packing the files is set to GMT or BST at the time of ZIP creation.
 * 
 *   - If set to GMT:
 *       - file A: 2020-03-29 00:59 (UTC+00:00)
 *       - file B: 2020-03-29 01:00 (UTC+00:00)
 *   - If set to BST:
 *       - file A: 2020-03-29 01:59 (UTC+01:00)
 *       - file B: 2020-03-29 02:00 (UTC+01:00)
 * 
 * It follows that you are unable to determine the actual last modified time
 * of any file stored in the ZIP archive, if you don't know the locale time
 * setting of the computer at the time it created the ZIP.
 * 
 * This format is used in some data formats from the MS-DOS era, for example:
 * 
 *   - [zip](/zip/)
 *   - [rar](/rar/)
 *   - [vfat](/vfat/) (FAT12)
 *   - [lzh](/lzh/)
 *   - [cab](http://justsolve.archiveteam.org/wiki/Cabinet)
 * \sa https://docs.microsoft.com/en-us/windows/win32/sysinfo/ms-dos-date-and-time Source
 * \sa https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-dosdatetimetofiletime Source
 * \sa https://github.com/reactos/reactos/blob/c6b6444/dll/win32/kernel32/client/time.c#L82-L87 DosDateTimeToFileTime
 * \sa https://download.microsoft.com/download/0/8/4/084c452b-b772-4fe5-89bb-a0cbf082286a/fatgen103.doc page 25/34
 */

class dos_datetime_t : public kaitai::kstruct {

public:
    class time_t;
    class date_t;

    dos_datetime_t(kaitai::kstream* p__io, kaitai::kstruct* p__parent = 0, dos_datetime_t* p__root = 0);

private:
    void _read();
    void _clean_up();

public:
    ~dos_datetime_t();

    class time_t : public kaitai::kstruct {

    public:

        time_t(kaitai::kstream* p__io, dos_datetime_t* p__parent = 0, dos_datetime_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~time_t();

    private:
        bool f_second;
        int32_t m_second;

    public:
        int32_t second();

    private:
        bool f_padded_second;
        std::string m_padded_second;

    public:
        std::string padded_second();

    private:
        bool f_padded_minute;
        std::string m_padded_minute;

    public:
        std::string padded_minute();

    private:
        bool f_padded_hour;
        std::string m_padded_hour;

    public:
        std::string padded_hour();

    private:
        uint64_t m_second_div_2;
        uint64_t m_minute;
        uint64_t m_hour;
        dos_datetime_t* m__root;
        dos_datetime_t* m__parent;

    public:
        uint64_t second_div_2() const { return m_second_div_2; }
        uint64_t minute() const { return m_minute; }
        uint64_t hour() const { return m_hour; }
        dos_datetime_t* _root() const { return m__root; }
        dos_datetime_t* _parent() const { return m__parent; }
    };

    class date_t : public kaitai::kstruct {

    public:

        date_t(kaitai::kstream* p__io, dos_datetime_t* p__parent = 0, dos_datetime_t* p__root = 0);

    private:
        void _read();
        void _clean_up();

    public:
        ~date_t();

    private:
        bool f_year;
        int32_t m_year;

    public:

        /**
         * only years from 1980 to 2107 (1980 + 127) can be represented
         */
        int32_t year();

    private:
        bool f_padded_day;
        std::string m_padded_day;

    public:
        std::string padded_day();

    private:
        bool f_padded_month;
        std::string m_padded_month;

    public:
        std::string padded_month();

    private:
        bool f_padded_year;
        std::string m_padded_year;

    public:
        std::string padded_year();

    private:
        uint64_t m_day;
        uint64_t m_month;
        uint64_t m_year_minus_1980;
        dos_datetime_t* m__root;
        dos_datetime_t* m__parent;

    public:
        uint64_t day() const { return m_day; }
        uint64_t month() const { return m_month; }
        uint64_t year_minus_1980() const { return m_year_minus_1980; }
        dos_datetime_t* _root() const { return m__root; }
        dos_datetime_t* _parent() const { return m__parent; }
    };

private:
    time_t* m_time;
    date_t* m_date;
    dos_datetime_t* m__root;
    kaitai::kstruct* m__parent;

public:
    time_t* time() const { return m_time; }
    date_t* date() const { return m_date; }
    dos_datetime_t* _root() const { return m__root; }
    kaitai::kstruct* _parent() const { return m__parent; }
};

#endif  // DOS_DATETIME_H_

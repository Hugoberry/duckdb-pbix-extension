cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)


# Set extension name here
set(TARGET_NAME pbix)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)


set(STRING_ENCODING_TYPE "ICONV" CACHE STRING "Set the way strings have to be encoded (ICONV|WIN32API|NONE|...)")
add_definitions(-DKS_STR_ENCODING_ICONV)


project(${TARGET_NAME})
include_directories(src/include)
include_directories(third_party/sqlite)
include_directories(third_party/tinyxml2)
include_directories(third_party/xpress9)
include_directories(third_party/kaitai)
include_directories(third_party)
include_directories(src/abf)

set(SQLITE_SOURCES
    third_party/sqlite/sqlite3.c
    third_party/sqlite/shell.c)

set(TINYXML2_SOURCES
    third_party/tinyxml2/tinyxml2.cpp)

set(XPRESS9_SOURCES
    third_party/xpress9/Xpress9DecHuffman.c
    third_party/xpress9/Xpress9DecLz77.c
    third_party/xpress9/Xpress9Misc.c
    third_party/xpress9/Xpress9Wrapper.cpp)

set(KAITAI_SOURCES
    third_party/kaitai/kaitaistream.cpp)

SET(ABF_SOURCES
    src/abf/abf_parser.cpp
    src/abf/backup_file.cpp
    src/abf/backup_log_header.cpp
    src/abf/backup_log.cpp
    src/abf/collations.cpp
    src/abf/crc32.c
    src/abf/file_group.cpp
    src/abf/file_list.cpp
    src/abf/languages.cpp
    src/abf/log_backup_file.cpp
    src/abf/virtual_directory.cpp
    src/abf/zip_utils.cpp)

set(EXTENSION_SOURCES 
    src/pbix_extension.cpp
    src/pbix_scanner.cpp
    src/pbix_reader.cpp
    src/sqlite_db.cpp 
    src/sqlite_stmt.cpp
    src/sqlite_utils.cpp
    src/vertipaq_decoder.cpp
    src/column_data_idfmeta.cpp
    src/column_data_idf.cpp
    src/column_data_dictionary.cpp
    src/huffman_decoder.cpp
    ${KAITAI_SOURCES}
    ${SQLITE_SOURCES}
    ${TINYXML2_SOURCES}
    ${XPRESS9_SOURCES}
    ${ABF_SOURCES})

# build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
# build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

add_library(${EXTENSION_NAME} STATIC ${EXTENSION_SOURCES})

set(PARAMETERS "-warnings")
build_loadable_extension(${TARGET_NAME} ${PARAMETERS} ${EXTENSION_SOURCES})



install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

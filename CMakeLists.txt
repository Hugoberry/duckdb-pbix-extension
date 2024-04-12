cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)


# Set extension name here
set(TARGET_NAME pbix)

set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

project(${TARGET_NAME})
include_directories(src/include)
include_directories(src/abf)
include_directories(third_party/miniz)
include_directories(third_party/sqlite)
include_directories(third_party/tinyxml2)
include_directories(third_party/xpress9)
include_directories(third_party)

set(MINIZ_SOURCES
    third_party/miniz/miniz.c)

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
    src/abf/AbfParser.cpp
    src/abf/BackupFile.cpp
    src/abf/BackupLog.cpp
    src/abf/BackupLogHeader.cpp
    src/abf/Collations.cpp
    src/abf/Crc32.c
    src/abf/FileGroup.cpp
    src/abf/FileGroups.cpp
    src/abf/FileList.cpp
    src/abf/Languages.cpp
    src/abf/LogBackupFile.cpp
    src/abf/VirtualDirectory.cpp
    src/abf/pbix.cpp)

set(EXTENSION_SOURCES 
    src/pbix_extension.cpp
    src/pbix_scanner.cpp
    src/sqlite_db.cpp 
    src/sqlite_stmt.cpp
    src/sqlite_utils.cpp
    ${MINIZ_SOURCES}
    ${SQLITE_SOURCES}
    ${TINYXML2_SOURCES}
    ${XPRESS9_SOURCES}
    ${KAITAI_SOURCES}
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

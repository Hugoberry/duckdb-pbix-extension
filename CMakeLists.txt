cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 17)


# Set extension name here
set(TARGET_NAME pbix)

# DuckDB's extension distribution supports vcpkg. As such, dependencies can be added in ./vcpkg.json and then
# used in cmake with find_package. Feel free to remove or replace with other dependencies.
# Note that it should also be removed from vcpkg.json to prevent needlessly installing it..
# find_package(OpenSSL REQUIRED)
# find_package(tinyxml2 REQUIRED)
find_package(unofficial-sqlite3 REQUIRED)
find_package(Crc32c REQUIRED)
find_package(miniz REQUIRED)

find_package(Threads REQUIRED)


set(EXTENSION_NAME ${TARGET_NAME}_extension)
set(LOADABLE_EXTENSION_NAME ${TARGET_NAME}_loadable_extension)

# set(STRING_ENCODING_TYPE "ICONV" CACHE STRING "Set the way strings have to be encoded (ICONV|WIN32API|NONE|...)")
# add_definitions(-DKS_STR_ENCODING_ICONV)


project(${TARGET_NAME})
include_directories(src/include)
include_directories(src/kaitai)
include_directories(src/xpress9)
include_directories(src/abf)
include_directories(src/third_party)

set(EXTENSION_SOURCES 
    src/abf/AbfParser.cpp
    src/abf/BackupFile.cpp
    src/abf/BackupLog.cpp
    src/abf/BackupLogHeader.cpp
    src/abf/Collations.cpp
    src/abf/FileGroup.cpp
    src/abf/FileGroups.cpp
    src/abf/FileList.cpp
    src/abf/Languages.cpp
    src/abf/LogBackupFile.cpp
    src/abf/VirtualDirectory.cpp
    src/pbix_extension.cpp
    src/pbix_scanner.cpp
    src/sqlite_db.cpp 
    src/sqlite_stmt.cpp
    src/sqlite_utils.cpp
    src/third_party/tinyxml2.cpp
    # src/third_party/sqlite3.c
    # src/third_party/shell.c
    src/xpress9/Xpress9DecHuffman.c
    src/xpress9/Xpress9DecLz77.c
    src/xpress9/Xpress9Misc.c
    src/xpress9/Xpress9Wrapper.cpp)

build_static_extension(${TARGET_NAME} ${EXTENSION_SOURCES})
build_loadable_extension(${TARGET_NAME} " " ${EXTENSION_SOURCES})

# Link OpenSSL in both the static library as the loadable extension
# target_link_libraries(${EXTENSION_NAME} OpenSSL::SSL OpenSSL::Crypto)
# target_link_libraries(${LOADABLE_EXTENSION_NAME} OpenSSL::SSL OpenSSL::Crypto)

# Right after defining and linking libraries to ${EXTENSION_NAME}
# target_link_libraries(${EXTENSION_NAME} tinyxml2::tinyxml2)
target_link_libraries(${EXTENSION_NAME}  unofficial::sqlite3::sqlite3)
target_link_libraries(${EXTENSION_NAME} Crc32c::crc32c)
target_link_libraries(${EXTENSION_NAME}  Threads::Threads)
target_link_libraries(${EXTENSION_NAME}  miniz::miniz)
# Add the include directories here
# target_include_directories(${EXTENSION_NAME} PRIVATE ${tinyxml2_INCLUDE_DIRS})

# And similarly for ${LOADABLE_EXTENSION_NAME}
# target_link_libraries(${LOADABLE_EXTENSION_NAME} tinyxml2::tinyxml2)
target_link_libraries(${LOADABLE_EXTENSION_NAME}  unofficial::sqlite3::sqlite3)
target_link_libraries(${LOADABLE_EXTENSION_NAME} Crc32c::crc32c)
target_link_libraries(${LOADABLE_EXTENSION_NAME}  Threads::Threads)
target_link_libraries(${LOADABLE_EXTENSION_NAME}  miniz::miniz)
# Add the include directories here
# target_include_directories(${LOADABLE_EXTENSION_NAME} PRIVATE ${tinyxml2_INCLUDE_DIRS})


install(
  TARGETS ${EXTENSION_NAME}
  EXPORT "${DUCKDB_EXPORT_SET}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}")

include_directories(include)
include_directories(kaitai)

add_library(
  pbix_ext_library OBJECT
  pbix_extension.cpp pbix_scanner.cpp sqlite_db.cpp sqlite_stmt.cpp
  sqlite_utils.cpp tinyxml2.cpp zip.cpp dos_datetime.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:pbix_ext_library>
    PARENT_SCOPE)
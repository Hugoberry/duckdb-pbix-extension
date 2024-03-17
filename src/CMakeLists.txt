include_directories(include)

add_subdirectory(abf)
add_subdirectory(xpress9)

add_library(
    xpress9wrapper_obj OBJECT
  pbix_extension.cpp )
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:pbix_ext_library>
    PARENT_SCOPE)
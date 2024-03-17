# Define an object library for xpress9-related files
add_library(xpress9wrapper_obj OBJECT
    src/xpress9/Xpress9DecHuffman.c
    src/xpress9/Xpress9DecLz77.c
    src/xpress9/Xpress9Misc.c
    src/xpress9/Xpress9Wrapper.cpp
)

# Optionally, set properties or include directories specific to this object library
target_include_directories(xpress9wrapper_obj PRIVATE ${PROJECT_SOURCE_DIR}/src/xpress9)

# Append these object files to the list in the parent scope
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:xpress9wrapper_obj>
    PARENT_SCOPE)

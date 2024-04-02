PROJ_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Configuration of extension
EXT_NAME=pbix
EXT_CONFIG=${PROJ_DIR}extension_config.cmake


# Setting CMAKE_PREFIX_PATH environment variable
# export CMAKE_PREFIX_PATH := vcpkg_installed/x64-linux:${CMAKE_PREFIX_PATH}
# export CMAKE_PREFIX_PATH := vcpkg_installed/x64-linux/share/kaitai-struct-cpp-stl-runtime:${CMAKE_PREFIX_PATH}
# export kaitai-struct-cpp-stl-runtime_DIR := /workspaces/duckdb-pbix-extension/build/release/vcpkg_installed/x64-linux/share/kaitai-struct-cpp-stl-runtime

# VCPKG_TOOLCHAIN_FILE=/workspaces/duckdb-pbix-extension/vcpkg/scripts/buildsystems/vcpkg.cmake


# Include the Makefile from extension-ci-tools
include extension-ci-tools/makefiles/duckdb_extension.Makefile
PROJ_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# Configuration of extension
EXT_NAME=pbix
EXT_CONFIG=${PROJ_DIR}extension_config.cmake


# Setting CMAKE_PREFIX_PATH environment variable
export CMAKE_PREFIX_PATH := vcpkg_installed/x64-linux/share/tinyxml2:${CMAKE_PREFIX_PATH}
export CMAKE_PREFIX_PATH := vcpkg_installed/x64-linux/share/unofficial-sqlite3:${CMAKE_PREFIX_PATH}

# Include the Makefile from extension-ci-tools
include extension-ci-tools/makefiles/duckdb_extension.Makefile
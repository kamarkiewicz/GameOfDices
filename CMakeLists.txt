cmake_minimum_required (VERSION 2.8)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
	"${CMAKE_SOURCE_DIR}/cmake_modules")

project (GameOfDices)
set (EXECUTABLE_NAME ${PROJECT_NAME})

# Version
set (GOD_VERSION_MAJOR 0)
set (GOD_VERSION_MINOR 1)

# Enable debug symbols by default
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
    # set(CMAKE_INSTALL_PREFIX DebugMe)
endif()

# Paths
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set (RESOURCE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/res)

# Deps
find_package (CSFML 2.1 COMPONENTS graphics window system audio REQUIRED)
find_package (Lua 5.1 REQUIRED)

# Compiler flags
set (CMAKE_C_FLAGS "-Wall -pipe -std=c99")
set (CMAKE_C_FLAGS_RELEASE "-Os")
set (CMAKE_C_FLAGS_DEBUG "-Wextra -g")

# Stuff
add_subdirectory ("src")
add_subdirectory ("res")

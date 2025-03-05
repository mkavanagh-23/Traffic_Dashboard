# Minimum CMake version supporting modern C++ features
cmake_minimum_required(VERSION 3.20)

# Project name and language
project(Traffic_Dashboard 
    LANGUAGES CXX
    VERSION 1.0.0
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable CMake to generate the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Ensure out-of-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

# Add support for multi-configuration generators
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

# Debug-specific compiler flags
set(CMAKE_CXX_FLAGS_DEBUG 
    "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -g -O0" 
    CACHE STRING "Flags used by the C++ compiler during debug builds" FORCE
)

# Release-specific compiler flags
set(CMAKE_CXX_FLAGS_RELEASE 
    "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3" 
    CACHE STRING "Flags used by the C++ compiler during release builds" FORCE
)

# Include CMake module for finding packages
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Attempt to find system libraries first
find_package(CURL QUIET)
find_package(jsoncpp QUIET)
find_package(RapidXML QUIET)

# External dependencies directory
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")

# Function to download and setup external libraries if not found
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# libcurl setup (only if not found in system)
if(NOT CURL_FOUND)
    message(STATUS "libcurl not found, will be downloaded and built")
    FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-8_4_0  # Use a specific stable version
        GIT_SHALLOW TRUE
    )
    
    # Explicitly configure libcurl build options
    set(BUILD_CURL_EXE OFF CACHE BOOL "")
    set(BUILD_TESTING OFF CACHE BOOL "")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "")  # Ensure shared library is built
    
    FetchContent_MakeAvailable(curl)
else()
    message(STATUS "libcurl found, using system version ${CURL_VERSION}")
endif()

# JsonCpp setup (only if not found in system)
if(NOT jsoncpp_FOUND)
    message(STATUS "JsonCpp not found, will be downloaded and built")
    FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
        GIT_TAG 1.9.5  # Use a stable version
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(jsoncpp)
else()
    message(STATUS "JsonCpp found, using system version ${jsoncpp_VERSION}")
endif()

# RapidXML setup (only if not found in system, note: header-only library)
if(NOT RapidXML_FOUND)
    message(STATUS "RapidXML not found, will be downloaded")
    FetchContent_Declare(
        rapidxml
        GIT_REPOSITORY https://github.com/discord/rapidxml.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(rapidxml)
else()
    message(STATUS "RapidXML found, using system version ${RapidXML_VERSION}")
endif()

# Collect source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

# Collect header files
file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Determine include paths for each library
if(CURL_FOUND)
    set(CURL_INCLUDE_DIRS "${CURL_INCLUDE_DIR}")
else()
    set(CURL_INCLUDE_DIRS "${curl_SOURCE_DIR}/include")
endif()

if(JSONCPP_FOUND)
    set(JSONCPP_INCLUDE_DIRS "${JSONCPP_INCLUDE_DIR}")
else()
    set(JSONCPP_INCLUDE_DIRS "${jsoncpp_SOURCE_DIR}/include")
endif()

if(RapidXML_FOUND)
    set(RAPIDXML_INCLUDE_DIRS "${RAPIDXML_INCLUDE_DIR}")
else()
    set(RAPIDXML_INCLUDE_DIRS "${rapidxml_SOURCE_DIR}")  # RapidXML is typically header-only
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/Data
    ${CMAKE_SOURCE_DIR}/include/Data/Traffic
    ${CURL_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${RAPIDXML_INCLUDE_DIRS}
)

# Link libraries (prefer system libraries if found, otherwise link built ones)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    ${CURL_LIBRARIES}      # Use system libcurl if found
    jsoncpp_lib            # JsonCpp library (system or built)
    ${RAPIDXML_LIBRARIES}  # RapidXML (usually header-only)
)

# Platform-specific configurations
if(MSVC)
    # Microsoft Visual C++
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    # GCC
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# If run with -install, install to system binary dir
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# Copy executable to project root after build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:${PROJECT_NAME}>
    ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}
    COMMENT "Copying binary to project root"
)

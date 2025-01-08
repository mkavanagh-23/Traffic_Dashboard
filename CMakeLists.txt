set(PROJECT_NAME Traffic_Dashboard)

# Enable CMake to generate the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

project(${PROJECT_NAME} VERSION 1.0.0)

set(SOURCES
    src/main.cpp
)

# Output final executable in project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# Include directories
target_include_directories(${PROJECT_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/external/videolan/libvlcpp
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBVLC REQUIRED libvlc)

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBVLC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBVLC_LIBRARIES})

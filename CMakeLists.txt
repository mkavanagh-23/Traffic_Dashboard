cmake_minimum_required(VERSION 3.16)

# Set the name for the project solution
set(PROJECT_NAME Traffic_Dashboard)
project(${PROJECT_NAME} VERSION 1.0.0)

# Set C++ version standard 
set(CMAKE_CXX_STANDARD 20)

# Enable CMake to generate the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.cpp
    src/Video.cpp
)

# Output final executable in project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

# Create the executable target and set compilation flags
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

# Include directories
target_include_directories(${PROJECT_NAME}
  PRIVATE ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/external/videolan/libvlcpp
)

# Find and link system libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBVLC REQUIRED libvlc)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBVLC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBVLC_LIBRARIES})

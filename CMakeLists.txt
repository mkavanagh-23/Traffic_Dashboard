cmake_minimum_required(VERSION 3.16)

# Set the name for the project solution
set(PROJECT_NAME Traffic_Dashboard)
project(${PROJECT_NAME} VERSION 1.0.0)

# Set C++ version standard 
set(CMAKE_CXX_STANDARD 20)

# Enable CMake to generate the compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp")

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
  PRIVATE ${PROJECT_SOURCE_DIR}/include/Data
  PRIVATE ${PROJECT_SOURCE_DIR}/include/Data/Traffic
  PRIVATE ${PROJECT_SOURCE_DIR}/external/videolan/libvlcpp
  PRIVATE ${PROJECT_SOURCE_DIR}/external/rapidxml
)

# Add third-party libraries
add_subdirectory(external/sdl2)
add_subdirectory(external/curl)
add_subdirectory(external/jsoncpp)

# Find and link system libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBVLC REQUIRED libvlc)
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBVLC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} 
  PRIVATE ${LIBVLC_LIBRARIES}
  PRIVATE SDL2
  PRIVATE CURL::libcurl
  PRIVATE jsoncpp
)

# Set rpath for SDL2 libraries on macOS
set(CMAKE_INSTALL_RPATH "@executable_path")
# For SDL2_image (or other libraries), you might also want to add it
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Link SDL2main if building for Windows
if(WIN32)
  target_link_libraries(
    ${PROJECT_NAME} PRIVATE SDL2main
  )
endif()

# Copy runtime DLLs to the output directory for dynamic libraries
if(TARGET SDL2)
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:SDL2>"
      "$<TARGET_FILE_DIR:Traffic_Dashboard>"
    VERBATIM
  )
endif()

if(TARGET CURL::libcurl)
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:CURL::libcurl>"
      "$<TARGET_FILE_DIR:Traffic_Dashboard>"
    VERBATIM
  )
endif()

if(TARGET jsoncpp)
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD COMMAND
    ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:jsoncpp>"
      "$<TARGET_FILE_DIR:Traffic_Dashboard>"
    VERBATIM
  )
endif()

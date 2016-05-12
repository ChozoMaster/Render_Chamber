cmake_minimum_required(VERSION 3.0)

project(glviz)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Visual studio solution directories.
set_property(GLOBAL PROPERTY USE_FOLDERS on)

# Enable simultaneous compilation of source files.
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# Put all executables and libraries into a common directory.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH    "${PROJECT_BINARY_DIR}/bin")

include(macros)

add_subdirectory(extern)
add_subdirectory(glviz)
add_subdirectory(Projects)

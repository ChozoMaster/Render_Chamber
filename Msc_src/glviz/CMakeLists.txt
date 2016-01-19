function(opengl_wrap_cpp _OUT SHADERS)
    foreach(SHADER ${SHADERS})
        get_filename_component(FILE ${SHADER} NAME_WE)
        get_filename_component(EXT ${SHADER} EXT)

        string(REGEX REPLACE "\\." "_" EXT "${EXT}")

        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/src/${FILE}${EXT}.cpp"
            COMMAND embed_resource ARGS "${FILE}${EXT}" "${SHADER}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/src"
            DEPENDS ${SHADER}
        )
        
        list(APPEND SOURCE "${CMAKE_CURRENT_BINARY_DIR}/src/${FILE}${EXT}.cpp")
    endforeach(SHADER)

    set(${_OUT} ${SOURCE} PARENT_SCOPE)
endfunction()

file(GLOB OPENGL_SHADERS src/shader/*)
opengl_wrap_cpp(SOURCES_OPENGL_SHADERS "${OPENGL_SHADERS}")

source_group("Shader Files" FILES ${OPENGL_SHADERS})

add_library(shader
    ${SOURCES_OPENGL_SHADERS}
    ${OPENGL_SHADERS}
)

add_dependencies(shader
    embed_resource
)

set(SOURCES
    src/glviz.cpp
    src/camera.cpp
    src/shader.cpp
    src/trackball.cpp
    src/program.cpp
    src/buffer.cpp
)

set(HEADERS
    src/glviz.hpp
    src/camera.hpp
    src/shader.hpp
    src/trackball.hpp
    src/program.hpp
    src/buffer.hpp
)

# Use static anttweakbar.
add_definitions(-DTW_NO_LIB_PRAGMA -DTW_STATIC)

# Use static GLEW.
add_definitions(-DGLEW_STATIC)

# Use static freeglut.
add_definitions(-DFREEGLUT_LIB_PRAGMAS=0 -DFREEGLUT_STATIC)

# Disable stupid MSVC warnings.
add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

add_library(glviz
    ${SOURCES}
    ${HEADERS}
)

find_package(OpenGL REQUIRED)

include_directories(
    include
    ${eigen_INCLUDE_DIRS}
    ${anttweakbar_INCLUDE_DIRS}
    ${freeglut_INCLUDE_DIRS}
    ${glew_INCLUDE_DIRS}
)

add_dependencies(glviz
    shader
    anttweakbar
    freeglut
    glew
)

set(glviz_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "glviz include directory")
set(glviz_LIBRARIES glviz shader
    CACHE INTERNAL "glviz library")

set(SOURCES
    src/LoadOGL.cpp
    src/LoadOGLCore.cpp
    src/TwBar.cpp
    src/TwColors.cpp
    src/TwEventGLFW.c
    src/TwEventGLUT.c
    src/TwEventSDL.c
    src/TwEventSDL12.c
    src/TwEventSDL13.c
    src/TwEventSFML.cpp
    src/TwEventWin.c
    src/TwFonts.cpp
    src/TwMgr.cpp
    src/TwOpenGL.cpp
    src/TwOpenGLCore.cpp
    src/TwPrecomp.cpp
)

set(HEADERS
    include/AntTweakBar.h
)

add_definitions(-DTW_EXPORTS -DTW_STATIC)

include_directories(include)

add_library(anttweakbar STATIC ${HEADERS} ${SOURCES})

set_target_properties(anttweakbar PROPERTIES FOLDER "External Dependencies")

set(anttweakbar_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "anttweakbar include directory")
set(anttweakbar_LIBRARIES anttweakbar
    CACHE INTERNAL "anttweakbar library")


get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Use static anttweakbar.
add_definitions(-DTW_NO_LIB_PRAGMA -DTW_STATIC)

# Use static GLEW.
add_definitions(-DGLEW_STATIC)

# Use static freeglut.
add_definitions(-DFREEGLUT_LIB_PRAGMAS=0 -DFREEGLUT_STATIC)

file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/resources/" GLVIZ_RESOURCES_DIR)
if (WIN32)
    string(REGEX REPLACE "\\\\" "\\\\\\\\"
        GLVIZ_RESOURCES_DIR ${GLVIZ_RESOURCES_DIR})
endif()
configure_file(config.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")

file(GLOB HEADERS "src/*.h")
file(GLOB SOURCES "src/*.cpp" "src/*.c")

INCLUDE_DIRECTORIES(${HEADERS})
add_executable(${PROJECT_NAME}
    ${SOURCES}
	${HEADERS}
)

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
message("OPENMP FOUND")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${glviz_INCLUDE_DIRS}
    ${eigen_INCLUDE_DIRS}
    ${anttweakbar_INCLUDE_DIRS}
    ${freeglut_INCLUDE_DIRS}
    ${glew_INCLUDE_DIRS}
)

add_dependencies(${PROJECT_NAME}
    glviz
)

target_link_libraries(${PROJECT_NAME}
    ${glviz_LIBRARIES}
    ${anttweakbar_LIBRARIES}
    ${freeglut_LIBRARIES}
    ${glew_LIBRARIES}
)

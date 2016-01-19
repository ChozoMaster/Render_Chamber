SET(PROJECT_NAME Voxelization_Project)
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

#########################
#### Setup for using Qt5
##########################



SET(BACKEND_HEADER
	"src/rply.h"
	"src/MyViz.h"
	"src/FileLoader.h"
	"src/parameters.h"
	"src/Utility.h"
	"src/Timer.h"
	"src/GPUTimer.h"
	"src/OctreeComputation.h"
	"src/md2model.h"

)

SET(BACKEND_SOURCE
	"src/parameters.cpp"
	"src/main.cpp"
	"src/MyViz.cpp"
	"src/OctreeComputation.cpp"
	"src/rply.c"
	"src/FileLoader.cpp"
	"src/Utility.cpp"
	"src/Timer.cpp"
	"src/GPUTimer.cpp"
	"src/md2model.cpp"
)

SET(SIMULATION_HEADER
	"src/Particle.h"
	"src/Integrator.h"
	"src/Rigidbody.h"
	"src/Spring.h"
	"src/Cuboid.h"
)

SET(SIMULATION_SOURCE
	"src/Particle.cpp"
	"src/Integrator.cpp"
	"src/Rigidbody.cpp"
	"src/Spring.cpp"
	"src/Cuboid.cpp"
)



#SOURCE_GROUP("Header Files\\Backend Headers" FILES ${BACKEND_HEADER})
#SOURCE_GROUP("Source Files\\Backend Source" FILES ${BACKEND_SOURCE})
SOURCE_GROUP("Simulation\\Source Files" FILES ${SIMULATION_SOURCE})
SOURCE_GROUP("Simulation\\Header Files" FILES ${SIMULATION_HEADER})

add_executable(${PROJECT_NAME}
    ${BACKEND_SOURCE}
	 ${BACKEND_HEADER}
	 ${SIMULATION_HEADER}
	 ${SIMULATION_SOURCE}
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

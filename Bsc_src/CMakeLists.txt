cmake_minimum_required(VERSION 2.6)
project(LiverVis)
SET(PROJECT_NAME LiverVis)

# Find VTK.
FIND_PACKAGE(VTK)
IF(VTK_FOUND)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
  MESSAGE(FATAL_ERROR
          "Cannot build without VTK. Please set VTK_DIR.")
ENDIF(VTK_FOUND)



#########################
#### Setup for using Qt4
##########################
#FIND_PACKAGE(Qt4 REQUIRED)
#SET (QT_USE_QTMAIN TRUE)
#MESSAGE(STATUS "QT4 libraries: ${QT_USE_FILE}")
# Add Qt4 include dir and libraries

#SET(QT_USE_QTOPENGL 1)
#INCLUDE(${QT_USE_FILE})
#ADD_DEFINITIONS(${QT_DEFINITIONS})
#INCLUDE_DIRECTORIES(${QT_INCLUDES})

#include(FindOpenMP)
#if(OPENMP_FOUND)
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()




SET(${PROJECT_NAME}_SOURCE
	"src/main.cpp"

)


SET(BACKEND_HEADER
	"header/IdDistanceTupel.h"
	"header/LiverDataLoader.h"
	"header/Liver.h"
	"header/RelativeDistance.h"
	"header/QtInterface.h"
	"header/ColorSource.h"
	"header/RenderTupel.h"
	"header/PCA.h"
	"header/AverageLiver.h"
	"header/Clustering.h"
	"header/vtkOFFReader.h"
	
)

SET(BACKEND_SOURCE
	"src/IdDistanceTupel.cpp"
	"src/Liver.cpp"
	"src/RelativeDistance.cpp"
	"src/QtInterface.cpp"
	"src/ColorSource.cpp"
	"src/RenderTupel.cpp"
	"src/LiverDataLoader.cpp"
	"src/PCA.cpp"
	"src/AverageLiver.cpp"
	"src/Clustering.cpp"
	"src/vtkOFFReader.cxx"
	)


SET(GUI_HEADER
	"header/mainWindow.h"
	"header/MouseInteractorStyle2.h"
	"header/HighlightInteractor.h"
	"header/ClusterHighlighting.h"
	"header/ClusterGraph.h"
	"header/ClusterScene.h"
	"header/MouseInteractorRubberBand.h"
	
)

SET(GUI_SOURCE
	"src/mainWindow.cpp"
	"src/MouseInteractorStyle2.cpp"
	"src/HighlightInteractor.cpp"
	"src/ClusterHighlighting.cpp"
	"src/ClusterGraph.cpp"
	"src/ClusterScene.cpp"
	"src/MouseInteractorRubberBand.cpp"
)

SET(VTK_DIR
	"/VTK_64bit_QT_build"
)

SET(${PROJECT_NAME}_MOC_HEADERS
	"header/mainWindow.h"
	"header/ClusterHighlighting.h"
	"header/QtInterface.h"
	"header/ClusterGraph.h"
	"header/Clustering.h"
	"header/ClusterScene.h"
	"header/PCA.h"
	"header/MouseInteractorStyle2.h"
)

SET(${PROJECT_NAME}_MOC_UI
	"res/mainWindow.ui"
	"res/clusterHighlightning.ui"
)

SET(${PROJECT_NAME}_MOC_RES
	"res/mainWindow.qrc"
)
QT4_WRAP_CPP(${PROJECT_NAME}_MOC_OUTFILES1 ${${PROJECT_NAME}_MOC_HEADERS})
QT4_WRAP_UI(${PROJECT_NAME}_MOC_OUTFILES2 ${${PROJECT_NAME}_MOC_UI})
QT4_ADD_RESOURCES(${PROJECT_NAME}_MOC_OUTFILES3 ${${PROJECT_NAME}_MOC_RES})

FOREACH(item ${${PROJECT_NAME}_MOC_OUTFILES2})
    SET(${PROJECT_NAME}_MOC_OUTFILES2_LAST ${item})
ENDFOREACH(item ${${PROJECT_NAME}_MOC_OUTFILES2})
GET_FILENAME_COMPONENT(MOC_OUTFILES2_PATH ${${PROJECT_NAME}_MOC_OUTFILES2_LAST} PATH)


SOURCE_GROUP("Header Files\\Backend Headers" FILES ${BACKEND_HEADER})
SOURCE_GROUP("Source Files\\Backend Source" FILES ${BACKEND_SOURCE})
SOURCE_GROUP("Header Files\\GUI Headers" FILES ${GUI_HEADER})
SOURCE_GROUP("Source Files\\GUI Source" FILES ${GUI_SOURCE})
SOURCE_GROUP("Generated" FILES ${${PROJECT_NAME}_MOC_OUTFILES1} ${${PROJECT_NAME}_MOC_OUTFILES2} ${${PROJECT_NAME}_MOC_OUTFILES3})





add_executable(${PROJECT_NAME} 
	${${PROJECT_NAME}_SOURCE}
	${BACKEND_SOURCE}
	${BACKEND_HEADER}
	${GUI_HEADER}
	${GUI_SOURCE}
	${${PROJECT_NAME}_MOC_OUTFILES1}
	${${PROJECT_NAME}_MOC_OUTFILES2}
	${${PROJECT_NAME}_MOC_OUTFILES3}
)

INCLUDE_DIRECTORIES(
	header
	${MOC_OUTFILES2_PATH}
	)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} 
vtkCommon
vtkFiltering
#vtkParallel
QVTK
${QT_LIBRARIES}
)



if ( NOT Q_WS_MAC)
    if (DEFINED QT_QMAKE_EXECUTABLE)
        SET (QTLIBLIST QtCore QtGui)

        IF (MSVC)
            set(TYPE "d")
            FOREACH(qtlib ${QTLIBLIST})
              IF (WIN32)
                GET_FILENAME_COMPONENT(QT_DLL_PATH_tmp
${QT_QMAKE_EXECUTABLE} PATH)
                file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug)
                file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Release)
                file(MAKE_DIRECTORY
${CMAKE_BINARY_DIR}/MinSizeRel)
                file(MAKE_DIRECTORY
${CMAKE_BINARY_DIR}/RelWithDebInfo)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}${type}d4.dll
                    DESTINATION ./
                    CONFIGURATIONS Debug
                    COMPONENT Applications)
                INSTALL(FILES ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                    DESTINATION ./
                    CONFIGURATIONS Release
                    COMPONENT Applications)
                add_custom_target(ZZ_${qtlib}-Debug-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E
copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}${TYPE}4.dll
                      ${CMAKE_BINARY_DIR}/Debug/
                            COMMENT "Copying ${qtlib}${TYPE}4.dll to
${CMAKE_BINARY_DIR}/Debug/")
                add_custom_target(ZZ_${qtlib}-Release-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E
copy_if_different ${QT_DLL_PATH_tmp}/${qtlib}4.dll
                            ${CMAKE_BINARY_DIR}/Release/
                            COMMENT "Copying ${qtlib}4.dll to
${CMAKE_BINARY_DIR}/Release/")
              ENDIF (WIN32)
            ENDFOREACH(qtlib)

        endif()
    endif(DEFINED QT_QMAKE_EXECUTABLE)
endif()



##Custom Target to copy QT dlls##
#add_custom_target(prepare ALL
#    COMMENT "Copying QT Dlls")

# Copy Qt runtime libs
#    set( QT_LIBS QtCore QtGui QtNetwork )

#add_custom_command(TARGET prepare POST_BUILD COMMAND "${CMAKE_COMMAND}" ARGS -E make_directory "Release/" )    
 #   foreach( lib ${QT_LIBS} )
 #       set( location location-NOTFOUND )
  #      find_file( location ${lib}4.dll PATHS ${QT_LIBRARY_DIR} )
  #     add_custom_command(TARGET prepare   POST_BUILD COMMAND "${CMAKE_COMMAND}" ARGS -E copy ${location} "Release/" )
  #  endforeach( lib )

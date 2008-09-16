project( ktechlab )
set( VERSION 0.5.0 )

find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)
include( ${CMAKE_SOURCE_DIR}/cmake/modules/FindGLIB.cmake )
include( ${CMAKE_SOURCE_DIR}/cmake/modules/FindGPSim.cmake )

#set(CMAKE_VERBOSE_MAKEFILE ON)

set( CMAKE_REQUIRED_FLAGS
    -fexceptions
    )
add_definitions(${QT4_DEFINITIONS} ${KDE4_DEFINITIONS} ${GLIB2_DEFINITIONS})

link_directories(${KDE4_LIB_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE4_INCLUDES} ${QT_INCLUDES} ${GLIB2_INCLUDE_DIRS} )

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h )

add_subdirectory( src )
add_subdirectory( icons )
#add_subdirectory( examples )
add_subdirectory( docs )

########### install files ###############


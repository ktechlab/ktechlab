project( ktechlab )
set( VERSION 0.3.6 )

find_package(Qt4 4.6.0 REQUIRED)
find_package(KDE4 REQUIRED)
include( ${CMAKE_SOURCE_DIR}/cmake/modules/FindGLIB.cmake )
include( ${CMAKE_SOURCE_DIR}/cmake/modules/FindGPSim.cmake )

#set(CMAKE_VERBOSE_MAKEFILE ON)

set( CMAKE_REQUIRED_FLAGS
	-fexceptions
	)
add_definitions(${QT_DEFINITIONS} ${KDE3_DEFINITIONS} ${GLIB2_DEFINITIONS})

link_directories(${KDE3_LIB_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS} )

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake
	${CMAKE_BINARY_DIR}/config.h )

add_subdirectory( microbe )
add_subdirectory( src )
add_subdirectory( examples )
add_subdirectory( icons )
add_subdirectory( po )
add_subdirectory( doc )

########### install files ###############




#original Makefile.am contents follow:

#AUTOMAKE_OPTIONS = foreign 1.5
#
#include admin/deps.am
#
#SUBDIRS=$(TOPSUBDIRS)

project( ktechlab )
set( VERSION 0.3.6 )

find_package(Qt4 4.6.0 REQUIRED)
find_package(KDE4 REQUIRED)
include( ${CMAKE_SOURCE_DIR}/cmake/modules/FindGLIB.cmake )
include( ${CMAKE_SOURCE_DIR}/cmake/modules/FindGPSim.cmake )

#set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions ( -DQT3_SUPPORT )
#add_definitions ( -DQT3_SUPPORT_WARNINGS )


set( CMAKE_REQUIRED_FLAGS
	-fexceptions
	)
add_definitions(${QT_DEFINITIONS} ${GLIB2_DEFINITIONS}${KDE4_DEFINITIONS})

# KDE disables STL compatibility by default; re-enabling it is supported
# see http://kde.6490.n7.nabble.com/QT-NO-STL-in-KDE4-td1123882i20.html
# and http://www.cmake.org/Wiki/HowToBuildKDE4Software#Some_useful_functions
remove_definitions(-DQT_NO_STL)

link_directories(${KDE4_LIB_DIR}) # not really needed, see FindKde4Internal.cmake

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${KDE4_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${GLIB2_INCLUDE_DIRS} )

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

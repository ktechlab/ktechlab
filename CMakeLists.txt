project( ktechlab )
set( VERSION 0.5.0 )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
cmake_minimum_required(VERSION 2.6)

find_package(KDE4 4.2.0 REQUIRED)
find_package(Qt4 4.6 REQUIRED)
find_package(KDevPlatform 0.10.0 REQUIRED)
find_package(GLIB REQUIRED)

include( KDE4Defaults )
include( MacroLibrary )

MACRO_OPTIONAL_FIND_PACKAGE( GPSim )

set(KDE4_BUILD_TESTS "ON" CACHE "BOOL" "Enable building of tests" FORCE )
include(KDevPluginVersion)

#set(CMAKE_VERBOSE_MAKEFILE ON)

set( CMAKE_REQUIRED_FLAGS
    -fexceptions
    )
add_definitions(${QT4_DEFINITIONS} ${KDE4_DEFINITIONS} ${GLIB2_DEFINITIONS})

link_directories(${KDE4_LIB_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE4_INCLUDES}
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${QT_INCLUDES}
    ${GLIB2_INCLUDE_DIRS}
)

CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h
)

add_subdirectory( src )
add_subdirectory( icons )
add_subdirectory( examples )
add_subdirectory( doc )
add_subdirectory( tests )

########### install files ###############


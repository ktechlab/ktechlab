project( ktechlab )
set( VERSION 0.5.0 )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
cmake_minimum_required(VERSION 2.8.9)

find_package(Qt4 4.6.0 REQUIRED)
find_package(GLIB REQUIRED)
find_package(KDE4 4.2.0)
find_package(KDevPlatform 0.10.0)
find_package( GPSim )

if(KDE4_FOUND AND KDEVPLATFORM_FOUND)
  set(KDEVPLATFORM_ENABLED 1)
else(KDE4_FOUND AND KDEVPLATFORM_FOUND)
endif(KDE4_FOUND AND KDEVPLATFORM_FOUND)


if(KDEVPLATFORM_ENABLED)

include( MacroLibrary )
include( KDE4Defaults )


set(KDE4_BUILD_TESTS "ON" CACHE "BOOL" "Enable building of tests" FORCE )
include(KDevPluginVersion)

add_definitions( -DKDEV_PLUGIN_VERSION=${KDEV_PLUGIN_VERSION})

else(KDEVPLATFORM_ENABLED)

endif(KDEVPLATFORM_ENABLED)

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



if(KDEVPLATFORM_ENABLED)
add_subdirectory( icons )
add_subdirectory( examples )
add_subdirectory( doc )
add_subdirectory( tests )
else(KDEVPLATFORM_ENABLED)

endif(KDEVPLATFORM_ENABLED)

add_subdirectory( src )

########### install files ###############

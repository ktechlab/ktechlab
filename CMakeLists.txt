#set( KTECHLAB_DEVELOPER_BUILD "true" )

project( ktechlab )
set( VERSION 0.40.0 )

cmake_minimum_required(VERSION 2.8)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(Qt4 4.6.0 REQUIRED)
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(GPSim)

#set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions ( -DQT3_SUPPORT )
#add_definitions ( -DQT3_SUPPORT_WARNINGS )

if (KTECHLAB_DEVELOPER_BUILD)
# for debugging with GCC 5
    add_definitions( -fno-omit-frame-pointer )
    set(SANITIZE_FLAGS " -fsanitize=address -fno-omit-frame-pointer ") # address
    #set(SANITIZE_FLAGS " -fsanitize=undefined -fno-omit-frame-pointer ") # undefined
    #set(SANITIZE_FLAGS "") # disabled

    add_definitions( ${SANITIZE_FLAGS} )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_FLAGS}" )

    #add_definitions( -Wold-style-cast ) # warn on old style casts. as these could introduce bugs

endif (KTECHLAB_DEVELOPER_BUILD)

add_definitions( -fno-inline )  # do not inline any method calls; this is useful for crash debugging
add_definitions(${QT_DEFINITIONS} ${GLIB2_DEFINITIONS} ${KDE4_DEFINITIONS})

# KDE disables STL compatibility by default; re-enabling it is supported
# see http://kde.6490.n7.nabble.com/QT-NO-STL-in-KDE4-td1123882i20.html
# and http://www.cmake.org/Wiki/HowToBuildKDE4Software#Some_useful_functions
remove_definitions(-DQT_NO_STL)

# TODO switch to NO_GPSIM to HAVE_GPSIM
if (NOT GPSim_FOUND)
    set(NO_GPSIM TRUE)
endif ()

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/src/core ${KDE4_INCLUDES})

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake
	${CMAKE_BINARY_DIR}/config.h )

add_subdirectory( microbe )
add_subdirectory( src )
add_subdirectory( examples )
add_subdirectory( icons )
add_subdirectory( icons-kde3 )
add_subdirectory( doc )

add_subdirectory(tests)

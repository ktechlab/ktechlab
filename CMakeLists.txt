set( KTECHLAB_DEVELOPER_BUILD "true" )

project( ktechlab )
set( VERSION 0.50.0 )

cmake_minimum_required(VERSION 3.1)
# cmake CXX_STANDARD needs min cmake 3.1
# https://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake/20165220#20165220
#
# KDE libs need min C++11 capabile compiler, so C++ can be requiested
# https://community.kde.org/Frameworks/Policies#Frameworks_compiler_requirements_and_C.2B.2B11
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)


find_package(ECM 0.0.11 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(ECMInstallIcons)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package(Qt5 CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Test # TODO this should be optional
    )

find_package(KF5TextEditor)

find_package(KF5 REQUIRED COMPONENTS
    Config
    CoreAddons
    DocTools
    IconThemes
    I18n
    KDELibs4Support
    KHtml
    KIO
    Parts
    TextEditor
    TextWidgets
    Solid
    WidgetsAddons
    XmlGui
    )

# find_package(Qt4 4.6.0 REQUIRED)
# find_package(KDE4 REQUIRED)
# include(KDE4Defaults)


# Edit FindGPSim and replace
#    include(MacroPushRequiredVars) -> include(CMakePushCheckState)
#    macro_push_required_vars() -> cmake_push_check_state()
#    macro_pop_required_vars() -> cmake_pop_check_state()
find_package(GPSim REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-Wno-deprecated-declarations) # TODO too much spam for now
add_definitions(-fPIC)

#set(CMAKE_VERBOSE_MAKEFILE ON)

#add_definitions ( -DQT3_SUPPORT )
#add_definitions ( -DQT3_SUPPORT_WARNINGS )

if (KTECHLAB_DEVELOPER_BUILD)
# for debugging with GCC 5
    add_definitions( -fno-omit-frame-pointer )
    set(SANITIZE_FLAGS " -fsanitize=address -fno-omit-frame-pointer -fPIC") # address
    #set(SANITIZE_FLAGS " -fsanitize=undefined -fno-omit-frame-pointer ") # undefined
    #set(SANITIZE_FLAGS "") # disabled

    #add_definitions ( -DQT3_SUPPORT_WARNINGS )

    add_definitions( ${SANITIZE_FLAGS} )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_FLAGS}" )

    add_definitions( -Wall -Wextra )

    #add_definitions( -Wold-style-cast ) # warn on old style casts. as these could introduce bugs

endif (KTECHLAB_DEVELOPER_BUILD)

add_definitions( -fno-inline )  # do not inline any method calls; this is useful for crash debugging
add_definitions(
    ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}
    ${GLIB2_DEFINITIONS} ${KDE4_DEFINITIONS})

message("Qt5Core_EXECUTABLE_COMPILE_FLAGS=>>>${Qt5Core_EXECUTABLE_COMPILE_FLAGS}<<<" )


# KDE disables STL compatibility by default; re-enabling it is supported
# see http://kde.6490.n7.nabble.com/QT-NO-STL-in-KDE4-td1123882i20.html
# and http://www.cmake.org/Wiki/HowToBuildKDE4Software#Some_useful_functions
remove_definitions(-DQT_NO_STL)

# TODO switch to NO_GPSIM to HAVE_GPSIM
if (NOT GPSim_FOUND)
    set(NO_GPSIM TRUE)
endif ()

message("Qt5Core_INCLUDE_DIRS=>>>${Qt5Core_INCLUDE_DIRS}<<<" )
message("Qt5Widgets_INCLUDE_DIRS=>>>${Qt5Widgets_INCLUDE_DIRS}<<<" )
message("KF5TextEditor_INCLUDE_DIRS=>>>${KF5TextEditor_INCLUDE_DIRS}<<<" )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src/core
    ${KDE4_INCLUDES}
#    ${KDELibs4Support_INCLUDE_DIRS}
    ${TextEditor_INCLUDE_DIRS}  # TODO figure out this one
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Test_INCLUDE_DIRS}
    )

set( SRC_EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples/ )
set( SRC_TESTS_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/data/ )

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake
	${CMAKE_BINARY_DIR}/config.h )


message(STATUS "include_dir begin")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "include_dir='${dir}'")
endforeach()
message(STATUS "include_dir end")

# message(STATUS "variables begin")
# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
# message(STATUS "variables end")

add_subdirectory( microbe )
add_subdirectory( src )
add_subdirectory( examples )
add_subdirectory( icons )
add_subdirectory( icons-kde3 )
add_subdirectory( doc )

add_subdirectory(tests)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

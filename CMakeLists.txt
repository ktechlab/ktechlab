# cmake CXX_STANDARD needs min cmake 3.1
# https://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake/20165220#20165220
cmake_minimum_required(VERSION 3.1)

#set( KTECHLAB_DEVELOPER_BUILD "true" )

project( ktechlab )
set( VERSION 0.50.0 )

# set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)

find_package(ECM 0.0.11 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(FeatureSummary)

if(BUILD_TESTING)
    set(QT_ADDITIONAL_COMPONENTS Test)
endif()
find_package(Qt5 CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    PrintSupport
    ${QT_ADDITIONAL_COMPONENTS}
    )

find_package(KF5 REQUIRED COMPONENTS
    Config
    CoreAddons
    DocTools
    IconThemes
    I18n
    KHtml
    KIO
    Parts
    TextEditor
    TextWidgets
    Solid
    WidgetsAddons
    WindowSystem
    XmlGui
    )

# Edit FindGPSim and replace
#    include(MacroPushRequiredVars) -> include(CMakePushCheckState)
#    macro_push_required_vars() -> cmake_push_check_state()
#    macro_pop_required_vars() -> cmake_pop_check_state()
find_package(GPSim)

add_definitions(-Wno-deprecated-declarations) # TODO too much spam for now
add_definitions(-fPIC)
add_definitions(
    -DQT_DISABLE_DEPRECATED_BEFORE=0 # even 0x050000 would be too high for now
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000 # to enable warnings for API since the disabled limit
#     -DQT_USE_QSTRINGBUILDER # TODO
#     -DQT_NO_CAST_TO_ASCII # TODO
#     -DQT_NO_CAST_FROM_ASCII # TODO
#     -DQT_NO_CAST_FROM_BYTEARRAY # TODO
#     -DQT_STRICT_ITERATORS # TODO
    -DQT_NO_URL_CAST_FROM_STRING
#     -DQT_NO_KEYWORDS # TODO
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
#    -DQT_NO_FOREACH # needed by KConfig system at least on Ubuntu 18.04
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x050000
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000 # to enable warnings for API since the disabled limit
)

#set(CMAKE_VERBOSE_MAKEFILE ON)

if (KTECHLAB_DEVELOPER_BUILD)
# for debugging with GCC 5
    add_definitions( -fno-omit-frame-pointer )
    set(SANITIZE_FLAGS " -fsanitize=address -fno-omit-frame-pointer -fPIC") # address
    #set(SANITIZE_FLAGS " -fsanitize=undefined -fno-omit-frame-pointer ") # undefined
    #set(SANITIZE_FLAGS "") # disabled

    #add_definitions ( -DQT3_SUPPORT_WARNINGS )

    add_definitions( ${SANITIZE_FLAGS} )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_FLAGS}" )

    #add_definitions( -Wold-style-cast ) # warn on old style casts. as these could introduce bugs

endif (KTECHLAB_DEVELOPER_BUILD)

add_definitions( -fno-inline )  # do not inline any method calls; this is useful for crash debugging
add_definitions(
    ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}
    ${GLIB2_DEFINITIONS}
)

message("Qt5Core_EXECUTABLE_COMPILE_FLAGS=>>>${Qt5Core_EXECUTABLE_COMPILE_FLAGS}<<<" )


# KDE disables STL compatibility by default; re-enabling it is supported
# see http://kde.6490.n7.nabble.com/QT-NO-STL-in-KDE4-td1123882i20.html
# and http://www.cmake.org/Wiki/HowToBuildKDE4Software#Some_useful_functions
remove_definitions(-DQT_NO_STL)

# TODO switch to NO_GPSIM to HAVE_GPSIM
if (NOT GPSim_FOUND)
    set(NO_GPSIM TRUE)
endif ()

include_directories(
    # for config.h file
    ${CMAKE_CURRENT_BINARY_DIR}
)

set( SRC_EXAMPLES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples/ )
set( SRC_TESTS_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/data/ )

CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/config.h.cmake
	${CMAKE_BINARY_DIR}/config.h )


# message(STATUS "include_dir begin")
# get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "include_dir='${dir}'")
# endforeach()
# message(STATUS "include_dir end")

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

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

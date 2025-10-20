cmake_minimum_required(VERSION 3.16)

#set( KTECHLAB_DEVELOPER_BUILD "true" )

project( ktechlab )
set( VERSION 0.51.0 )

# set(CMAKE_DEBUG_TARGET_PROPERTIES INCLUDE_DIRECTORIES)

find_package(ECM 6.0.0 CONFIG REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat OPTIONAL RESULT_VARIABLE KDEClangFormat_FOUND)
include(ECMSetupVersion)
include(ECMQtDeclareLoggingCategory)

include(ECMInstallIcons)
include(FeatureSummary)

ecm_setup_version(
    "${VERSION}"
    VARIABLE_PREFIX KTECHLAB
    VERSION_HEADER ktechlab_version.h
)

if(BUILD_TESTING)
    set(QT_ADDITIONAL_COMPONENTS Test)
endif()

find_package(Qt6 CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    PrintSupport
    SerialPort
    Core5Compat
    ${QT_ADDITIONAL_COMPONENTS}
    )

find_package(KF6 REQUIRED COMPONENTS
    Completion
    Config
    ConfigWidgets
    CoreAddons
    DocTools
    IconThemes
    I18n
    KIO
    Parts
    TextEditor
    TextWidgets
    WidgetsAddons
    WindowSystem
    XmlGui
    )

find_package(GPSim)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_definitions(
    -DQT_DISABLE_DEPRECATED_BEFORE=0x060000 # TODO target some reasonable version
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000 # to enable warnings for API since the disabled limit
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_CAST_TO_ASCII
#     -DQT_NO_CAST_FROM_ASCII # TODO
     -UQT_NO_CAST_FROM_ASCII # TODO
     -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_STRICT_ITERATORS
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    -DQT_NO_FOREACH
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x060000
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000 # to enable warnings for API since the disabled limit
)

#set(CMAKE_VERBOSE_MAKEFILE ON)

if (KTECHLAB_DEVELOPER_BUILD)
# for debugging with GCC + ASAN
    add_definitions( -fno-omit-frame-pointer )
    set(SANITIZE_FLAGS " -fsanitize=address -fno-omit-frame-pointer -fPIC") # address
    #set(SANITIZE_FLAGS " -fsanitize=undefined -fno-omit-frame-pointer ") # undefined
    #set(SANITIZE_FLAGS "") # disabled

    add_definitions( ${SANITIZE_FLAGS} )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZE_FLAGS}" )

    #add_definitions( -Wold-style-cast ) # warn on old style casts. as these could introduce bugs

endif ()

add_definitions( -fno-inline )  # do not inline any method calls; this is useful for crash debugging

# TODO switch to NO_GPSIM to HAVE_GPSIM
if (NOT GPSim_FOUND)
    set(NO_GPSIM TRUE)
endif ()

if (UNIX AND NOT APPLE)
    set(WITH_X11 TRUE)
endif ()

include_directories(
    # for config.h file
    ${CMAKE_CURRENT_BINARY_DIR}
)

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

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# add clang-format target for all our real source files
if(KDEClangFormat_FOUND)
    file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/*.cpp src/*.h)
    kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
endif()

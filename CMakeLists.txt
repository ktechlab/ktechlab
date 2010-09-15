project( ktechlab_minimal_simulator )
set( VERSION 0.5.0 )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(GLIB REQUIRED)

set( CMAKE_REQUIRED_FLAGS
    -fexceptions
    )
add_definitions(
    ${QT4_DEFINITIONS}
    ${GLIB2_DEFINITIONS}
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/languages
    ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics
    ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flowparts
    ${CMAKE_CURRENT_SOURCE_DIR}/src/math/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/physics
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${GLIB2_INCLUDE_DIRS}

    ${QT_QTTEST_INCLUDE_DIR}
)

CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h
)

# add_subdirectory( testing )

ADD_DEFINITIONS(-DQT3_SUPPORT)

set( CompileSimulator_SRCS
# ${CMAKE_SOURCE_DIR}/src/electronics/gpsimprocessor.cpp
 ${CMAKE_SOURCE_DIR}/src/simulator.cpp
)

qt4_automoc(${CompileSimulator_SRCS})


add_executable( CompiledSimulator
${CompileSimulator_SRCS}
)


target_link_libraries( CompiledSimulator
${QT_LIBRARIES}
${QT_QT3SUPPORT_LIBRARY}
${QT_QTTEST_LIBRARIES}

)

enable_testing()

add_test(CompiledSimulator CompiledSimulator)


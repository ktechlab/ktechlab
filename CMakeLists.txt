project( ktechlab_minimal_simulator )
set( VERSION 0.5.0 )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE debug)

find_package(Qt4 REQUIRED)
# find_package(GLIB REQUIRED)

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
ADD_DEFINITIONS(-DNO_GPSIM)

set( CompileSimulator_SRCS
 ${CMAKE_SOURCE_DIR}/testing/simulatortest.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/gpsimprocessor.cpp
 ${CMAKE_SOURCE_DIR}/src/simulator.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/logic.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/element.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/elementset.cpp
 ${CMAKE_SOURCE_DIR}/src/math/quickvector.cpp
 ${CMAKE_SOURCE_DIR}/src/math/quickmatrix.cpp
 ${CMAKE_SOURCE_DIR}/src/math/matrix.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/circuit.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/pin.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/wire.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/gpsimprocessor.cpp
 # support for elements
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/nonlinear.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/reactive.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/elementsignal.cpp
 # elements
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/bjt.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/capacitance.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/cccs.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/ccvs.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/currentsignal.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/currentsource.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/diode.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/inductance.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/jfet.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/mosfet.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/opamp.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/vccs.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/vcvs.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/voltagepoint.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/voltagesignal.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/voltagesource.cpp
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


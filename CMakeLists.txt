project( ktechlab_minimal_simulator )
set( VERSION 0.5.0 )

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
cmake_minimum_required(VERSION 2.6)

set(CMAKE_BUILD_TYPE debug)

find_package(Qt4 REQUIRED)
find_package(GLIB REQUIRED)
find_package(GPSim REQUIRED)
find_package(KDE4 REQUIRED)


set( CMAKE_REQUIRED_FLAGS
    -fexceptions
    )

# list(APPEND CMAKE_CXX_FLAGS "-fexceptions")

add_definitions(
    ${QT4_DEFINITIONS}
    ${GLIB2_DEFINITIONS}
    ${CMAKE_REQUIRED_FLAGS}
    )

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/languages
    ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics
    ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components
    ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/flowparts
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/math/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/physics
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_INCLUDES}
    ${GLIB2_INCLUDE_DIRS}
    ${KDE4_INCLUDES}
    ${QT_QTTEST_INCLUDE_DIR}
)

CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/config.h.cmake
    ${CMAKE_BINARY_DIR}/config.h
)

# add_subdirectory( testing )

ADD_DEFINITIONS(-DQT3_SUPPORT)
# ADD_DEFINITIONS(-DNO_GPSIM)

set( CompileSimulator_SRCS
 ${CMAKE_SOURCE_DIR}/testing/simulatortest.cpp
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
 ${CMAKE_SOURCE_DIR}/src/electronics/elementmap.cpp
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
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/resistance.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/vccs.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/vcvs.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/voltagepoint.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/voltagesignal.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/simulation/voltagesource.cpp
 # gpsim stuff
 ${CMAKE_SOURCE_DIR}/src/electronics/gpsimprocessor.cpp
 ${CMAKE_SOURCE_DIR}/src/debugmanager.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/sourceline.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/asmparser.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/language.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/processchain.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/externallanguage.cpp
 ${CMAKE_SOURCE_DIR}/src/gui/logview.cpp
 ${CMAKE_SOURCE_DIR}/src/gui/outputmethoddlg.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/microlibrary.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/micropackage.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/microinfo.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/asminfo.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/picinfo.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/picinfo12bit.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/picinfo14bit.cpp
 ${CMAKE_SOURCE_DIR}/src/micro/picinfo16bit.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/sdcc.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/picprogrammer.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/gplib.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/gplink.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/gpasm.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/microbe.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/gpdasm.cpp
 ${CMAKE_SOURCE_DIR}/src/languages/qprocesswitharguments.cpp
 # ${CMAKE_SOURCE_DIR}/src/languages/flowcode.cpp
 # support class for components
 ${CMAKE_SOURCE_DIR}/src/variant.cpp
 # base class for components
 ${CMAKE_SOURCE_DIR}/src/electronics/component.cpp
 ${CMAKE_SOURCE_DIR}/src/node.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/ecnode.cpp
 ${CMAKE_SOURCE_DIR}/src/connector.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/electronicconnector.cpp
 # components:
 ${CMAKE_SOURCE_DIR}/src/electronics/components/addac.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/bidirled.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/binarycounter.cpp
   # bussplitter.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/capacitor.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/demultiplexer.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/dependentsource.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/discretelogic.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/led.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecdiode.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecvoltagesource.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ec555.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecbcdto7segment.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecbjt.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecclockinput.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/eccurrentsignal.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/eccurrentsource.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecdiode.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecfixedvoltage.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecground.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecjfet.cpp
 # ${CMAKE_SOURCE_DIR}/src/electronics/components/eckeypad.cpp # interactive..
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecmosfet.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecopamp.cpp
 # ${CMAKE_SOURCE_DIR}/src/electronics/components/ecpotentiometer.cpp # interactive
 # ${CMAKE_SOURCE_DIR}/src/electronics/components/ecsevensegment.cpp # interactive
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecsignallamp.cpp
 # ${CMAKE_SOURCE_DIR}/src/electronics/components/ecsubcircuit.cpp # complicated
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecvoltagesignal.cpp
 ${CMAKE_SOURCE_DIR}/src/electronics/components/ecvoltagesource.cpp
 # ${CMAKE_SOURCE_DIR}/src/electronics/components/externalconnection.cpp # complicated
  # ..
 ${CMAKE_SOURCE_DIR}/src/electronics/components/resistor.cpp
)

qt4_automoc(${CompileSimulator_SRCS})


add_executable( compiled_simulator
${CompileSimulator_SRCS}
)


target_link_libraries( compiled_simulator
${QT_LIBRARIES}
${QT_QT3SUPPORT_LIBRARY}
${QT_QTTEST_LIBRARIES}
${GPSIM_LIBRARY}
${GPSim_LIBRARY}
${KDE4_KDEUI_LIBS}
)

enable_testing()

add_test(compiled_simulator compiled_simulator)


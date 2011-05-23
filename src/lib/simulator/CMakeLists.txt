
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
#    ${QT_INCLUDES}
#    ${GLIB2_INCLUDE_DIRS}
#    ${QT_QTTEST_INCLUDE_DIR}
)

# we need Qt3 support in some classes... 
# TODO make the code compile without
#      this flag turned on
ADD_DEFINITIONS(-DQT3_SUPPORT)
# ADD_DEFINITIONS(-DNO_GPSIM)

add_definitions(-fexceptions)

set( simulator_SRCS
 ${CMAKE_CURRENT_SOURCE_DIR}/src/ktlconfig.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/simulator.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/logic.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/element.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/elementset.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/math/quickvector.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/math/quickmatrix.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/math/matrix.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/circuit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/pin.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/wire.cpp
 # support for elements
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/nonlinear.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/reactive.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/elementsignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/elementmap.cpp
 # elements
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/bjt.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/capacitance.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/cccs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/ccvs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/currentsignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/currentsource.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/diode.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/inductance.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/jfet.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/mosfet.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/opamp.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/resistance.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/vccs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/vcvs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/voltagepoint.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/voltagesignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/simulation/voltagesource.cpp
 # gpsim stuff
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/gpsimprocessor.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/debugmanager.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/sourceline.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/asmparser.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/language.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/processchain.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/externallanguage.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/messageinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/outputmethodinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/microlibrary.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/micropackage.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/microinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/asminfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/picinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/picinfo12bit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/picinfo14bit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/micro/picinfo16bit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/sdcc.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/picprogrammer.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/gplib.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/gplink.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/gpasm.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/microbe.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/gpdasm.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/qprocesswitharguments.cpp
 # ${CMAKE_CURRENT_SOURCE_DIR}/src/languages/flowcode.cpp
 # support class for components
 ${CMAKE_CURRENT_SOURCE_DIR}/src/variant.cpp
 # base class for components
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/component.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/node.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/ecnode.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/connector.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/electronicconnector.cpp
 # components:
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/addac.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/bidirled.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/binarycounter.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/bussplitter.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/capacitor.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/demultiplexer.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/dependentsource.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/discretelogic.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ec555.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecbcdto7segment.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecbjt.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecclockinput.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/eccurrentsignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/eccurrentsource.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecdiode.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecfixedvoltage.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecground.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecjfet.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/eckeypad.cpp # interactive..
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecmosfet.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecopamp.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecpotentiometer.cpp # interactive
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecsevensegment.cpp # interactive
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecsignallamp.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecsubcircuit.cpp # complicated
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecvoltagesignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ecvoltagesource.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/externalconnection.cpp # complicated
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/flipflop.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/fulladder.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/inductor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ledbargraphdisplay.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/led.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/magnitudecomparator.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/matrixdisplay.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/matrixdisplaydriver.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/meter.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/multiinputgate.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/multiplexer.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/parallelportcomponent.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/piccomponent.cpp # needs porting
# ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/piccomponentpin.cpp # needs porting
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/probe.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/pushswitch.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/ram.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/resistor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/resistordip.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/rotoswitch.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/serialportcomponent.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/toggleswitch.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/variablecapacitor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/variableresistor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/src/electronics/components/voltageregulator.cpp

)

# add_library( ktlsimulator SHARED ${simulator_SRCS} )

# TODO make the library compile without KDE4 dependencies
#       maybe ship the kde4macros.cmake file with the source tree
kde4_add_library( ktlsimulator SHARED ${simulator_SRCS} )

target_link_libraries( ktlsimulator
 ${QT_QTCORE_LIBRARY}
 ${QT_LIBRARIES}
 ${QT_QTGUI_LIBRARY} # FIXME only for QColor in Variant
 ${QT_QT3SUPPORT_LIBRARY}
 ${GPSIM_LIBRARY}
 ${GPSim_LIBRARY}
)

install(TARGETS ktlsimulator ${INSTALL_TARGETS_DEFAULT_ARGS})

#qt4_automoc(
# creatingtest.h
# simulatortest.h
# ${SIMULATOR_LIB_SRCS}
# ${CompileSimulator_SRCS}
# ${qprocess_with_argument_SRC}
# ${CREATING_TEST_SRCS}
#)

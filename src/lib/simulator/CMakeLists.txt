
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/languages
    ${CMAKE_CURRENT_SOURCE_DIR}/electronics
    ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components
    ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/flowparts
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/math/
    ${CMAKE_CURRENT_SOURCE_DIR}/micro/
    ${CMAKE_CURRENT_SOURCE_DIR}/physics
    ${CMAKE_CURRENT_BINARY_DIR}
#    ${QT_INCLUDES}
#    ${GLIB2_INCLUDE_DIRS}
#    ${QT_QTTEST_INCLUDE_DIR}
)

add_definitions(-fexceptions)

set( simulator_SRCS
 ${CMAKE_CURRENT_SOURCE_DIR}/ktlconfig.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/simulator.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/logic.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/element.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/elementset.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/math/quickvector.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/math/quickmatrix.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/math/matrix.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/circuit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/pin.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/wire.cpp
 # support for elements
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/nonlinear.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/reactive.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/elementsignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/elementmap.cpp
 # elements
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/bjt.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/capacitance.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/cccs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/ccvs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/currentsignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/currentsource.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/diode.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/inductance.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/jfet.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/mosfet.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/opamp.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/resistance.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/vccs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/vcvs.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/voltagepoint.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/voltagesignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation/voltagesource.cpp
 # gpsim stuff
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/gpsimprocessor.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/debugmanager.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/sourceline.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/asmparser.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/language.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/processchain.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/externallanguage.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/messageinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/outputmethodinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/microlibrary.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/micropackage.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/microinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/asminfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/picinfo.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/picinfo12bit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/picinfo14bit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/micro/picinfo16bit.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/sdcc.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/picprogrammer.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/gplib.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/gplink.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/gpasm.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/microbe.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/gpdasm.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/languages/qprocesswitharguments.cpp
 # ${CMAKE_CURRENT_SOURCE_DIR}/languages/flowcode.cpp
 # support class for components
 ${CMAKE_CURRENT_SOURCE_DIR}/variant.cpp
 # base class for components
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/component.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/ecnode.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/connector.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/electronicconnector.cpp
 # components:
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/addac.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/bidirled.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/binarycounter.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/bussplitter.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/capacitor.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/demultiplexer.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/dependentsource.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/discretelogic.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ec555.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecbcdto7segment.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecbjt.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecclockinput.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/eccurrentsignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/eccurrentsource.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecdiode.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecfixedvoltage.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecground.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecjfet.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/eckeypad.cpp # interactive..
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecmosfet.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecopamp.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecpotentiometer.cpp # interactive
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecsevensegment.cpp # interactive
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecsignallamp.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecsubcircuit.cpp # complicated
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecvoltagesignal.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ecvoltagesource.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/externalconnection.cpp # complicated
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/flipflop.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/fulladder.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/inductor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ledbargraphdisplay.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/led.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/magnitudecomparator.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/matrixdisplay.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/matrixdisplaydriver.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/meter.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/multiinputgate.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/multiplexer.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/parallelportcomponent.cpp
# ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/piccomponent.cpp # needs porting
# ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/piccomponentpin.cpp # needs porting
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/probe.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/pushswitch.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/ram.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/resistor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/resistordip.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/rotoswitch.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/serialportcomponent.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/toggleswitch.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/variablecapacitor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/variableresistor.cpp
#${CMAKE_CURRENT_SOURCE_DIR}/electronics/components/voltageregulator.cpp

)

# if(KDEVPLATFORM_ENABLED)

kde4_add_library( ktlsimulator SHARED ${simulator_SRCS} )

install(TARGETS ktlsimulator ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries( ktlsimulator
 PUBLIC
 ${QT_QTCORE_LIBRARY}
 ${QT_LIBRARIES}
# ${QT_QT3SUPPORT_LIBRARY}
 ${GPSIM_LIBRARY}
)

# else(KDEVPLATFORM_ENABLED)

add_library( ktlsimulator_static STATIC ${simulator_SRCS} )

qt4_automoc( ${simulator_SRCS} )

# install(TARGETS ktlsimulator_static
#   RUNTIME DESTINATION bin
#   LIBRARY DESTINATION lib
#   ARCHIVE DESTINATION lib
# )

# target_link_libraries( ktlsimulator_qt
#  ${QT_QTCORE_LIBRARY}
#  ${QT_LIBRARIES}
#  ${QT_QT3SUPPORT_LIBRARY}
#  ${GPSIM_LIBRARY}
# )

# endif(KDEVPLATFORM_ENABLED)



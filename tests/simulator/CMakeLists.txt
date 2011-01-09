
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/lib
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_BUILD_DIR}
)

kde4_add_unit_test( basictest
    basictest.cpp
    ${CMAKE_SOURCE_DIR}/src/simulationmanager.cpp
    ${CMAKE_SOURCE_DIR}/src/interfaces/simulator/isimulationmanager.cpp
)

target_link_libraries( basictest
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
)

kde4_add_unit_test( addcomponents
    addcomponents.cpp
    ${CMAKE_SOURCE_DIR}/src/interfaces/simulator/isimulationmanager.cpp
    ${CMAKE_SOURCE_DIR}/src/simulationmanager.cpp
)

target_link_libraries( addcomponents
    ktlinterfaces
    ktlcircuit
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
)

kde4_add_unit_test( directsimulatortest
   directsimulatortest.cpp
   ${CMAKE_SOURCE_DIR}/src/interfaces/simulator/isimulator.cpp
   ${CMAKE_SOURCE_DIR}/src/simulationmanager.cpp
   ${CMAKE_SOURCE_DIR}/src/plugins/simulator/circuittransientsimulator.cpp
   ${CMAKE_SOURCE_DIR}/src/plugins/simulator/pingroup.cpp
   ${CMAKE_SOURCE_DIR}/src/plugins/simulator/elementset.cpp
   ${CMAKE_SOURCE_DIR}/src/plugins/basic_ec/elements/capacitance.cpp
   ${CMAKE_SOURCE_DIR}/src/plugins/basic_ec/elements/resistance.cpp
   ${CMAKE_SOURCE_DIR}/src/plugins/basic_ec/elements/voltagesource.cpp
)

target_link_libraries( directsimulatortest
    ktlinterfaces
    ktlmath
    ktlcircuit
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
)
add_definitions( "-DPROJECTS_SOURCE_DIR=\\\"${CMAKE_SOURCE_DIR}/tests/data\\\"" )

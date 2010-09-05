
include_directories(
    ${CMAKE_SOURCE_DIR}/src
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
    ${CMAKE_SOURCE_DIR}/src/simulationmanager.cpp
)

target_link_libraries( addcomponents
    ktlinterfaces
#   ktlcircuit_lib
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
)

add_definitions( "-DPROJECTS_SOURCE_DIR=\\\"${CMAKE_SOURCE_DIR}/tests/data\\\"" )

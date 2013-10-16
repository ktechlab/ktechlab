
include_directories(
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/src/interfaces/simulator/
)

# test for simulation manager

set( SIMULATIONMANAGER_TEST_SRCS
 simulationmanager_test.cpp
 ${CMAKE_SOURCE_DIR}/src/interfaces/simulator/isimulationmanager.cpp
)


set( SIMULATIONMANAGER_TEST_TOMOC
  simulationmanager_test.h
  ${CMAKE_SOURCE_DIR}/src/interfaces/simulator/isimulationmanager.h
)

qt4_wrap_cpp(SIMULATIONMANAGER_TEST_MOC ${SIMULATIONMANAGER_TEST_TOMOC} )

add_executable( simulationmanager_test
 ${SIMULATIONMANAGER_TEST_SRCS}
 ${SIMULATIONMANAGER_TEST_MOC}
)

target_link_libraries( simulationmanager_test
 ${QT_QTCORE_LIBRARIES}
 ${QT_QTTEST_LIBRARIES}
 ${KDEVPLATFORM_TESTS_LIBRARIES}
)

add_test(simulationmanager_test simulationmanager_test)

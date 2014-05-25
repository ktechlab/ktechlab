# this is the directory, where a test project is located
add_definitions( -DPROJECTS_SOURCE_DIR="${CMAKE_SOURCE_DIR}/tests/data" )

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/lib
    ${CMAKE_SOURCE_DIR}/src/plugins
)

set(circuitModelTest_SRCS
    circuitmodeltest.cpp
)

kde4_add_unit_test(circuitmodeltest ${circuitModelTest_SRCS})

target_link_libraries( circuitmodeltest
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
    ktlcircuit
    ktlinterfaces
)


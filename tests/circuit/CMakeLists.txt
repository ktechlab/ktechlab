# this is the directory, where a test project is located
add_definitions( "-DPROJECTS_SOURCE_DIR=\\\"${CMAKE_SOURCE_DIR}/tests/data\\\"" )

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/plugins
)

set(circuitPlugin_SRCS
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/circuitmodel.cpp
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/circuitdocument.cpp
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/circuitview.cpp
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/circuitscene.cpp
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/componentitem.cpp
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/pinitem.cpp
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/theme.cpp
    ${CMAKE_SOURCE_DIR}/src/plugins/circuit/ktlcircuitplugin.cpp
)

set(circuitModelTest_SRCS
    circuitmodeltest.cpp
    ${circuitPlugin_SRCS}
)

kde4_add_unit_test(circuitmodeltest ${circuitModelTest_SRCS})

target_link_libraries( circuitmodeltest
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
    ktlinterfaces
)


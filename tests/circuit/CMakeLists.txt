# this is the directory, where a test project is located
if (${CMAKE_MAJOR_VERSION} LESS "3")
  add_definitions( -DPROJECTS_SOURCE_DIR=\\"${CMAKE_SOURCE_DIR}/tests/data\\" )
else (${CMAKE_MAJOR_VERSION} LESS "3")
  add_definitions( -DPROJECTS_SOURCE_DIR="${CMAKE_SOURCE_DIR}/tests/data" )
endif (${CMAKE_MAJOR_VERSION} LESS "3")

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


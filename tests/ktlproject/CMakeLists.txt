# this is the directory, where a test project is located
if (${CMAKE_MAJOR_VERSION} LESS "3")
  add_definitions( -DPROJECTS_SOURCE_DIR=\\"${CMAKE_SOURCE_DIR}/tests/data\\" )
else (${CMAKE_MAJOR_VERSION} LESS "3")
  add_definitions( -DPROJECTS_SOURCE_DIR="${CMAKE_SOURCE_DIR}/tests/data" )
endif (${CMAKE_MAJOR_VERSION} LESS "3")


include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_BUILD_DIR}
)

kde4_add_unit_test( ktlprojecttest ktlprojecttest.cpp
    ${CMAKE_SOURCE_DIR}/src/simulationmanager.cpp
    ${CMAKE_SOURCE_DIR}/src/interfaces/simulator/isimulationmanager.cpp
)

target_link_libraries( ktlprojecttest
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_PROJECT_LIBRARIES}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
)

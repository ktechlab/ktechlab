# this is the directory, where a test project is located
if (${CMAKE_MAJOR_VERSION} LESS "3")
  add_definitions( -DPROJECTS_SOURCE_DIR=\\"${CMAKE_SOURCE_DIR}/tests/data\\" )
else (${CMAKE_MAJOR_VERSION} LESS "3")
  add_definitions( "-DPROJECTS_SOURCE_DIR='${CMAKE_SOURCE_DIR}/tests/data'" )
endif (${CMAKE_MAJOR_VERSION} LESS "3")


include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_BUILD_DIR}
    ${CMAKE_SOURCE_DIR}/src/
)

kde4_add_unit_test(automatic_routertest automaticroutertest.cpp)

target_link_libraries( automatic_routertest
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
    ktlinterfaces
)



set (KDE4_BUILD_TESTS ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

kde4_add_unit_test( 
    circuitcreate 
    circuitcreate.cpp
)

target_link_libraries( 
    circuitcreate
    ktlinterfaces
    ktlcircuit_lib
    ${QT_QTTEST_LIBRARY} 
    ${KDEVPLATFORM_TESTS_LIBRARIES}
)


# this is the directory, where a test project is located
add_definitions( -DPROJECTS_SOURCE_DIR="${CMAKE_SOURCE_DIR}/tests/data" )

include_directories(
    ${CMAKE_SOURCE_DIR}/src/lib/simulator
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/languages
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/electronics
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/electronics/components
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/electronics/simulation
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/flowparts
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/gui
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/math/
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/micro/
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/physics
)
# simulator test

set( CompileSimulator_SRCS
 simulatortest.cpp
)

qt4_wrap_cpp( simulatortest_MOCFILES simulatortest.h )

add_executable( compiled_simulator
 ${CompileSimulator_SRCS}
 ${simulatortest_MOCFILES}
 )

# kde4_add_unit_test(compiled_simulator ${CompileSimulator_SRCS})

target_link_libraries( compiled_simulator
 ${QT_QTCORE_LIBRARIES}
 ${QT_QTTEST_LIBRARIES}
 ktlsimulator
)

 add_test(compiled_simulator compiled_simulator)



# test for qprocess with argument class

set( qprocess_with_argument_SRC
 testqprocesswitharguments.cpp
 ${CMAKE_SOURCE_DIR}/src/lib/simulator/languages/qprocesswitharguments.cpp
)

qt4_wrap_cpp( qprocess_with_argument_MOCFILES
 testqprocesswitharguments.h
 ${CMAKE_SOURCE_DIR}/src/lib/simulator/languages/qprocesswitharguments.h
)

add_executable( qprocess_with_args_test
 ${qprocess_with_argument_SRC}
 ${qprocess_with_argument_MOCFILES}
 )

target_link_libraries( qprocess_with_args_test
 ${QT_LIBRARIES}
 ${QT_QTCORE_LIBRARIES}
 ${QT_QTTEST_LIBRARIES}
 )
 
add_test(qprocess_with_args_test qprocess_with_args_test)

# test for proper destruction

set( CREATING_TEST_SRCS
 creatingtest.cpp
)

qt4_wrap_cpp(CREATING_TEST_MOC creatingtest.h)

add_executable( creating_test
 ${CREATING_TEST_SRCS}
 ${CREATING_TEST_MOC}
)

target_link_libraries( creating_test
 ktlsimulator
 ${QT_QTCORE_LIBRARIES}
 ${QT_QTTEST_LIBRARIES}
)

add_test(creating_test creating_test)

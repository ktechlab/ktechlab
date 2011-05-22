
include_directories(
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/languages
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/electronics
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/electronics/components
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/electronics/simulation
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/flowparts
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/gui
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/math/
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/micro/
    ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/physics
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
 ${QT_QTTEST_LIBRARIES}
 ktlsimulator
)

 add_test(compiled_simulator compiled_simulator)



# test for qprocess with argument class

set( qprocess_with_argument_SRC
 testqprocesswitharguments.cpp
 ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/languages/qprocesswitharguments.cpp
)

qt4_wrap_cpp( qprocess_with_argument_MOCFILES
 testqprocesswitharguments.h
 ${CMAKE_SOURCE_DIR}/src/lib/simulator/src/languages/qprocesswitharguments.h
)

add_executable( qprocess_with_args_test
 ${qprocess_with_argument_SRC}
 ${qprocess_with_argument_MOCFILES}
 )

target_link_libraries( qprocess_with_args_test
 ${QT_LIBRARIES}
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
 ${QT_QTTEST_LIBRARIES}
)



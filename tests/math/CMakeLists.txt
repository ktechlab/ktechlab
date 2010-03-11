include_directories(
   ${CMAKE_SOURCE_DIR}/src/math/
   ${EIGEN2_INCLUDE_DIR}
)

find_package( Eigen2 )

set( mathtest_SRCS
#	mathtest.cpp
   matrixtester.cpp
)

kde4_add_unit_test( mathtest TESTNAME mathtest ${mathtest_SRCS} )

target_link_libraries( mathtest
	ktlmath
	${QT_QTTEST_LIBRARY}
)

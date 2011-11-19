find_package( Eigen2 )

if( EIGEN2_FOUND )
   include_directories(
      ${CMAKE_SOURCE_DIR}/src/math/
      ${EIGEN2_INCLUDE_DIR}
   )

   set( mathtest_SRCS
      matrixtester.cpp
   )
   kde4_add_unit_test( mathtest TESTNAME mathtest ${mathtest_SRCS} )

   target_link_libraries( mathtest
	ktlmath
        ${QT_QTCORE_LIBRARIES}
        ${QT_QTTEST_LIBRARIES}
)
endif( EIGEN2_FOUND )


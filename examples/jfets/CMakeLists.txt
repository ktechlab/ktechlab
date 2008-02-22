include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	inverter.circuit
	DESTINATION share/apps/ktechlab/examples/jfets)



#original Makefile.am contents follow:

#examplesdir = $(kde_datadir)/ktechlab/examples/jfets
#examples_DATA = inverter.circuit

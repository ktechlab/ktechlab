include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	internals.circuit
	square-wave.circuit
	DESTINATION share/apps/ktechlab/examples/555)



#original Makefile.am contents follow:

#examples_DATA = internals.circuit square-wave.circuit
#examplesdir = $(kde_datadir)/ktechlab/examples/555

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	decoupler.circuit
	inverting-amplifier.circuit
	noninverting-amplifier.circuit
	instrumentation-amplifier.circuit
	DESTINATION share/apps/ktechlab/examples/opamps)



#original Makefile.am contents follow:

#examplesdir = $(kde_datadir)/ktechlab/examples/opamps
#examples_DATA = decoupler.circuit inverting-amplifier.circuit noninverting-amplifier.circuit instrumentation-amplifier.circuit

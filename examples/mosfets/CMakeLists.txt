include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	and.circuit
	inverter.circuit
	nand.circuit
	nor.circuit
	or.circuit
	DESTINATION share/apps/ktechlab/examples/mosfets)



#original Makefile.am contents follow:

#examplesdir = $(kde_datadir)/ktechlab/examples/mosfets
#examples_DATA = and.circuit inverter.circuit nand.circuit nor.circuit or.circuit

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES  
	2-way-switch.circuit
	3-way-switch.circuit
	capacitor.circuit
	lrc.circuit resistors-parallel.circuit
	resistors-series.circuit
	voltage-divider.circuit
	DESTINATION ${DATA_INSTALL_DIR}/ktechlab/examples/basic)



#original Makefile.am contents follow:

#examplesdir = $(kde_datadir)/ktechlab/examples/basic
#examples_DATA = 2-way-switch.circuit 3-way-switch.circuit capacitor.circuit\
#	lrc.circuit resistors-parallel.circuit resistors-series.circuit\
#	voltage-divider.circuit

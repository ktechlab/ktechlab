include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	astable-multivibrator.circuit
	inverter.circuit nand.circuit
	nor.circuit
	scmitt-trigger.circuit
	switch.circuit
	DESTINATION ${DATA_INSTALL_DIR}/ktechlab/examples/transistors)



#original Makefile.am contents follow:

#examplesdir = $(kde_datadir)/ktechlab/examples/transistors
#examples_DATA = astable-multivibrator.circuit inverter.circuit nand.circuit\
#	nor.circuit scmitt-trigger.circuit switch.circuit

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	full-wave-rectifier.circuit
	half-wave-rectifier.circuit
	DESTINATION ${DATA_INSTALL_DIR}/ktechlab/examples/diodes)



#original Makefile.am contents follow:

#examplesdir = $(kde_datadir)/ktechlab/examples/diodes
#examples_DATA = full-wave-rectifier.circuit half-wave-rectifier.circuit

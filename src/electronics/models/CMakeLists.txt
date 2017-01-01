include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	transistors_lib.txt
	DESTINATION ${DATA_INSTALL_DIR}/ktechlab/models)



#original Makefile.am contents follow:

#modelsdir = $(kde_datadir)/ktechlab/models
#models_DATA = transistors_lib.txt

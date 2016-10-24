include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	matrix_display.circuit
	DESTINATION share/apps/ktechlab/examples/matrix_disp_driver)

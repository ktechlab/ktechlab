include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############

install(FILES
	help-en_US
	help-es
	help-fr
	help-it
	help-de
	help-uk
	ohm.png
	ohm_unit.png
	opamp.png
	siemens.png
	siemens_unit.png
	tf_op.png
	DESTINATION share/apps/ktechlab/contexthelp)



#original Makefile.am contents follow:

#
#METASOURCES = AUTO
#contexthelp_DATA = help-en_US help-es help-fr help-it help-de ohm.png ohm_unit.png opamp.png siemens.png siemens_unit.png tf_op.png
#contexthelpdir = $(kde_datadir)/ktechlab/contexthelp

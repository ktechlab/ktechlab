add_subdirectory(pics)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############


kde3_install_icons(hicolor)



#original Makefile.am contents follow:

#
#METASOURCES = AUTO
#	
#KDE_ICON = AUTO
#
#SUBDIRS = pics

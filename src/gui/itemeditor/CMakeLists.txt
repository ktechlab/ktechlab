include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/src/drawparts ${CMAKE_SOURCE_DIR}/src/electronics ${CMAKE_SOURCE_DIR}/src/electronics/components ${CMAKE_SOURCE_DIR}/src/electronics/simulation ${CMAKE_SOURCE_DIR}/src/flowparts ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/gui/itemeditor ${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/mechanics ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_SOURCE_DIR}/gpsim-interface ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(itemeditor_STAT_SRCS
   itemeditor.cpp
   orientationwidget.cpp
   propertyeditor.cpp
   propertyeditorfile.cpp
   propertyeditorinput.cpp
   propertyeditoritem.cpp
   propertyeditorlist.cpp
   propertysubeditor.cpp
   propertyeditorcolor.cpp
   componentmodelwidget.cpp
)

qt4_automoc(${itemeditor_STAT_SRCS})

add_library(itemeditor STATIC ${itemeditor_STAT_SRCS})
target_link_libraries( itemeditor electronics )

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/core \
#	-I$(top_srcdir)/src/drawparts -I$(top_srcdir)/src/electronics \
#	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/simulation \
#	-I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/gui/itemeditor \
#	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro \
#	-I$(top_srcdir)/gpsim-interface $(all_includes)
#METASOURCES = AUTO
#noinst_LTLIBRARIES = libitemeditor.la
#libitemeditor_la_SOURCES = itemeditor.cpp orientationwidget.cpp \
#		propertyeditor.cpp propertyeditorfile.cpp propertyeditorinput.cpp propertyeditoritem.cpp \
#		propertyeditorlist.cpp propertysubeditor.cpp propertyeditorcolor.cpp \
#	componentmodelwidget.cpp
#
#libgui_la_PCH = AUTO
#			 
#
#noinst_HEADERS = componentmodelwidget.h

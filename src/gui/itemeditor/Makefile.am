INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/core \
	-I$(top_srcdir)/src/drawparts -I$(top_srcdir)/src/electronics \
	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/simulation \
	-I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/gui/itemeditor \
	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro \
	-I$(top_srcdir)/gpsim-interface $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES = libitemeditor.la
libitemeditor_la_SOURCES = itemeditor.cpp orientationwidget.cpp \
		propertyeditor.cpp propertyeditorfile.cpp propertyeditorinput.cpp propertyeditoritem.cpp \
		propertyeditorlist.cpp propertysubeditor.cpp propertyeditorcolor.cpp \
	componentmodelwidget.cpp

libgui_la_PCH = AUTO
			 

noinst_HEADERS = componentmodelwidget.h

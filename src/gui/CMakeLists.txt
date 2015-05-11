add_subdirectory(itemeditor)

include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/core
	${CMAKE_SOURCE_DIR}/src/electronics
	${CMAKE_SOURCE_DIR}/src/electronics/components
	${CMAKE_SOURCE_DIR}/src/electronics/simulation
	${CMAKE_SOURCE_DIR}/src/flowparts ${CMAKE_SOURCE_DIR}/src/gui
	${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/mechanics
	${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_SOURCE_DIR}/gpsim-interface
	${CMAKE_BINARY_DIR}/src/core
	${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(gui_STAT_SRCS
   contexthelp.cpp
   doublespinbox.cpp
   itemselector.cpp
   microsettingsdlg.cpp
   newfiledlg.cpp
   outputmethoddlg.cpp
   settingsdlg.cpp
   oscilloscope.cpp
   oscilloscopeview.cpp
   probepositioner.cpp
   logview.cpp
   projectdlgs.cpp
   microselectwidget.cpp
   symbolviewer.cpp
   programmerdlg.cpp
   colorcombo.cpp
   scopescreenview.cpp
   scopescreen.cpp
   scopeviewbase.cpp
   richtexteditor.cpp
   lineedit.cpp
)

qt4_automoc(${gui_STAT_SRCS})

set(gui_UI
    asmformattingwidget.ui
    gpasmsettingswidget.ui
    logicwidget.ui
    newfilewidget.ui
    newprojectwidget.ui
    outputmethodwidget.ui
    microsettingswidget.ui
    oscilloscopewidget.ui
    generaloptionswidget.ui
    createsubprojectwidget.ui
    processingoptionswidget.ui
    sdccoptionswidget.ui
    linkeroptionswidget.ui
    picprogrammerconfigwidget.ui
    newpinmappingwidget.ui
    programmerwidget.ui
    contexthelpwidget.ui
    scopescreenwidget.ui
    gplinksettingswidget.ui
)

kde4_add_ui_files(gui_STAT_SRCS ${gui_UI})

add_library(gui STATIC ${gui_STAT_SRCS})
target_link_libraries( itemeditor )

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/core \
#	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components \
#	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui \
#	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro \
#	-I$(top_srcdir)/gpsim-interface $(all_includes)
#METASOURCES = AUTO
#noinst_LTLIBRARIES = libgui.la
#libgui_la_SOURCES = contexthelp.cpp doublespinbox.cpp itemselector.cpp \
#	microsettingsdlg.cpp newfiledlg.cpp outputmethoddlg.cpp asmformattingwidget.ui \
#	gpasmsettingswidget.ui logicwidget.ui newfilewidget.ui newprojectwidget.ui \
#	outputmethodwidget.ui microsettingswidget.ui settingsdlg.cpp oscilloscope.cpp \
#	oscilloscopewidget.ui oscilloscopeview.cpp probepositioner.cpp generaloptionswidget.ui \
#	logview.cpp createsubprojectwidget.ui processingoptionswidget.ui \
#	sdccoptionswidget.ui projectdlgs.cpp linkeroptionswidget.ui microselectwidget.cpp \
#	symbolviewer.cpp picprogrammerconfigwidget.ui newpinmappingwidget.ui programmerwidget.ui \
#	programmerdlg.cpp colorcombo.cpp contexthelpwidget.ui scopescreenwidget.ui \
#	scopescreenview.cpp scopescreen.cpp scopeviewbase.cpp richtexteditor.cpp lineedit.cpp
#
#libgui_la_PCH = AUTO
#			 
#noinst_HEADERS = settingsdlg.h oscilloscope.h oscilloscopeview.h \
#	probepositioner.h projectdlgs.h microselectwidget.h symbolviewer.h programmerdlg.h \
#	colorcombo.h scopescreenview.h scopescreen.h scopeviewbase.h richtexteditor.h \
#	lineedit.h
#SUBDIRS = itemeditor
#libgui_la_LIBADD = $(top_builddir)/src/gui/itemeditor/libitemeditor.la

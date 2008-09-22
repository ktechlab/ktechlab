INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/core \
	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components \
	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui \
	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro \
	-I$(top_srcdir)/gpsim-interface $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES = libgui.la
libgui_la_SOURCES = asmformattingwidget.ui colorcombo.cpp contexthelp.cpp \
	contexthelpwidget.ui createsubprojectwidget.ui doublespinbox.cpp generaloptionswidget.ui \
	gpasmsettingswidget.ui gplinksettingswidget.ui itemselector.cpp lineedit.cpp \
	linkeroptionswidget.ui logicwidget.ui logview.cpp microselectwidget.cpp microsettingsdlg.cpp \
	microsettingswidget.ui newfiledlg.cpp newfilewidget.ui newpinmappingwidget.ui \
	newprojectwidget.ui oscilloscope.cpp oscilloscopeview.cpp oscilloscopewidget.ui \
	outputmethoddlg.cpp outputmethodwidget.ui picprogrammerconfigwidget.ui probepositioner.cpp \
	processingoptionswidget.ui programmerdlg.cpp programmerwidget.ui projectdlgs.cpp \
	richtexteditor.cpp scopescreen.cpp scopescreenview.cpp scopescreenwidget.ui \
	scopeviewbase.cpp sdccoptionswidget.ui settingsdlg.cpp symbolviewer.cpp

libgui_la_PCH = AUTO
			 
noinst_HEADERS = settingsdlg.h oscilloscope.h oscilloscopeview.h \
	probepositioner.h projectdlgs.h microselectwidget.h symbolviewer.h programmerdlg.h \
	colorcombo.h scopescreenview.h scopescreen.h scopeviewbase.h richtexteditor.h \
	lineedit.h
SUBDIRS = itemeditor
libgui_la_LIBADD = $(top_builddir)/src/gui/itemeditor/libitemeditor.la

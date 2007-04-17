INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/core \
	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components \
	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui \
	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro \
	-I$(top_srcdir)/gpsim-interface $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES = libgui.la
libgui_la_SOURCES = contexthelp.cpp doublespinbox.cpp itemselector.cpp \
	microsettingsdlg.cpp newfiledlg.cpp outputmethoddlg.cpp asmformattingwidget.ui \
	gpasmsettingswidget.ui logicwidget.ui newfilewidget.ui newprojectwidget.ui \
	outputmethodwidget.ui microsettingswidget.ui settingsdlg.cpp oscilloscope.cpp \
	oscilloscopewidget.ui oscilloscopeview.cpp probepositioner.cpp generaloptionswidget.ui \
	logview.cpp createsubprojectwidget.ui processingoptionswidget.ui \
	sdccoptionswidget.ui projectdlgs.cpp linkeroptionswidget.ui microselectwidget.cpp \
	symbolviewer.cpp picprogrammerconfigwidget.ui newpinmappingwidget.ui programmerwidget.ui \
	programmerdlg.cpp colorcombo.cpp contexthelpwidget.ui scopescreenwidget.ui \
	scopescreenview.cpp scopescreen.cpp scopeviewbase.cpp richtexteditor.cpp lineedit.cpp

libgui_la_PCH = AUTO
			 
noinst_HEADERS = settingsdlg.h oscilloscope.h oscilloscopeview.h \
	probepositioner.h projectdlgs.h microselectwidget.h symbolviewer.h programmerdlg.h \
	colorcombo.h scopescreenview.h scopescreen.h scopeviewbase.h richtexteditor.h \
	lineedit.h
SUBDIRS = itemeditor
libgui_la_LIBADD = $(top_builddir)/src/gui/itemeditor/libitemeditor.la

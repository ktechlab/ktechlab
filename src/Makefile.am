## Makefile.am for ktechlab

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ktechlab

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/drawparts \
	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components \
	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui \
	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(all_includes)

# the library search path. 
ktechlab_LDFLAGS = -module $(all_libraries) $(KDE_RPATH)

# the libraries to link against.
ktechlab_LDADD = $(top_builddir)/src/math/libtechmath.la \
	$(top_builddir)/src/gui/libgui.la $(top_builddir)/src/micro/libmicro.la \
	$(top_builddir)/src/flowparts/libflowparts.la $(top_builddir)/src/mechanics/libmechanics.la \
	$(top_builddir)/src/electronics/libelectronics.la $(top_builddir)/src/electronics/simulation/libelements.la \
	$(top_builddir)/src/electronics/components/libcomponents.la $(top_builddir)/src/languages/liblanguages.la \
	$(top_builddir)/src/drawparts/libdrawparts.la $(top_builddir)/src/core/libcore.la -lkutils $(LIB_GPSIM) -lktexteditor \
	$(LIB_KFILE) $(LIB_KDEPRINT)

# which sources should be compiled for ktechlab
ktechlab_SOURCES = ktechlab.cpp node.cpp connector.cpp itemlibrary.cpp \
			libraryitem.cpp projectmanager.cpp picitem.cpp variant.cpp canvasitemparts.cpp \
			flowcontainer.cpp microsettings.cpp fpnode.cpp cells.cpp asmformatter.cpp conrouter.cpp \
			nodegroup.cpp canvasmanipulator.cpp iteminterface.cpp itemgroup.cpp ciwidgetmgr.cpp \
			filemetainfo.cpp resizeoverlay.cpp document.cpp view.cpp docmanager.cpp cnitem.cpp \
			item.cpp cnitemgroup.cpp itemview.cpp itemdocument.cpp textview.cpp \
			textdocument.cpp circuitdocument.cpp flowcodedocument.cpp icnview.cpp icndocument.cpp \
			viewcontainer.cpp circuitview.cpp flowcodeview.cpp eventinfo.cpp oscilloscopedata.cpp \
			itemdocumentdata.cpp docmanageriface.cpp documentiface.cpp viewiface.cpp \
			docmanageriface.skel viewiface.skel documentiface.skel simulator.cpp katemdi.cpp \
			debugmanager.cpp recentfilesaction.cpp variablelabel.cpp

ktechlab_PCH = AUTO


# these are the headers for your project
noinst_HEADERS = ktechlab.h node.h connector.h itemlibrary.h libraryitem.h \
			projectmanager.h picitem.h canvasitemparts.h microsettings.h fpnode.h cells.h \
			asmformatter.h conrouter.h nodegroup.h canvasmanipulator.h iteminterface.h itemgroup.h \
			ciwidgetmgr.h filemetainfo.h resizeoverlay.h document.h view.h docmanager.h cnitem.h \
			item.h cnitemgroup.h itemview.h itemdocument.h textview.h textdocument.h \
			circuitdocument.h flowcodedocument.h icnview.h icndocument.h viewcontainer.h \
			circuitview.h flowcodeview.h eventinfo.h oscilloscopedata.h itemdocumentdata.h \
			docmanageriface.h documentiface.h viewiface.h simulator.h katemdi.h debugmanager.h \
		recentfilesaction.h variablelabel.h


# client stuff


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO


messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui" -o -name "*.kcfg"` > rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
		$(XGETTEXT) $$LIST -o $(podir)/ktechlab.pot; \
	fi

#KDE_ICON = ktechlab
#KDE_ICON = AUTO

# this is where the kdelnk file will go 
kdelnkdir = $(kde_appsdir)/Development
kdelnk_DATA = ktechlab.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/ktechlab
rc_DATA = ktechlabui.rc error_messages_en_gb ktechlabcircuitui.rc \
	ktechlabflowcodeui.rc ktechlabitemviewui.rc ktechlabmechanicsui.rc ktechlabtextui.rc ktechlabkateui.rc
 
SUBDIRS = core gui flowparts micro mechanics electronics languages drawparts \
	math
#iconsdir = $(kde_datadir)/ktechlab/icons


mimedir = $(kde_mimedir)/application
mime_DATA = x-circuit.desktop x-flowcode.desktop x-ktechlab.desktop x-microbe.desktop
EXTRA_DIST = $(mime_DATA)

katesyntaxdir = $(kde_datadir)/katepart/syntax
katesyntax_DATA = microbe.xml


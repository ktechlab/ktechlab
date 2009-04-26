## Makefile.am for ktechlab

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = ktechlab

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/doc -I$(top_srcdir)/microbe -I$(top_srcdir)/src \
	-I$(top_srcdir)/src/core -I$(top_srcdir)/src/drawparts -I$(top_srcdir)/src/electronics \
	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/models \
	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui \
	-I$(top_srcdir)/src/gui/itemeditor -I$(top_srcdir)/src/languages -I$(top_srcdir)/src/math \
	-I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(all_includes)

# the library search path. 
ktechlab_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
ktechlab_LDADD = $(top_builddir)/src/core/libcore.la \
	$(top_builddir)/src/drawparts/libdrawparts.la $(top_builddir)/src/electronics/components/libcomponents.la \
	$(top_builddir)/src/electronics/simulation/libelements.la $(top_builddir)/src/electronics/libelectronics.la \
	$(top_builddir)/src/flowparts/libflowparts.la $(top_builddir)/src/gui/itemeditor/libitemeditor.la \
	$(top_builddir)/src/gui/libgui.la $(top_builddir)/src/languages/liblanguages.la \
	$(top_builddir)/src/math/libtechmath.la $(top_builddir)/src/mechanics/libmechanics.la \
	$(top_builddir)/src/micro/libmicro.la $(LIB_GPSIM) $(LIB_KDEPRINT) $(LIB_KFILE) -lkhtml -lktexteditor \
	-lkutils

# which sources should be compiled for ktechlab
ktechlab_SOURCES = asmformatter.cpp canvas.cpp canvasitemparts.cpp \
	canvasmanipulator.cpp canvastip.cpp cells.cpp circuitview.cpp ciwidgetmgr.cpp cnitem.cpp \
	cnitemgroup.cpp connector.cpp connectorline.cpp conrouter.cpp debugmanager.cpp \
	docmanager.cpp docmanageriface.cpp docmanageriface.skel document.cpp documentiface.cpp \
	documentiface.skel eventinfo.cpp filemetainfo.cpp flowcodedocument.cpp flowcodeview.cpp \
	flowcontainer.cpp fpnode.cpp icndocument.cpp icnview.cpp item.cpp itemdocument.cpp \
	itemdocumentdata.cpp itemgroup.cpp iteminterface.cpp itemlibrary.cpp itemview.cpp \
	katemdi.cpp ktechlab.cpp ktlcanvas.cpp libraryitem.cpp microsettings.cpp node.cpp \
	nodegroup.cpp oscilloscopedata.cpp picitem.cpp projectmanager.cpp \
	recentfilesaction.cpp resizeoverlay.cpp simulator.cpp textdocument.cpp textview.cpp \
	variablelabel.cpp variant.cpp view.cpp viewcontainer.cpp viewiface.cpp viewiface.skel

ktechlab_PCH = AUTO


# these are the headers for your project
noinst_HEADERS = asmformatter.h canvas.h canvas_private.h canvasitemparts.h \
	canvasmanipulator.h canvastip.h cells.h circuitview.h ciwidgetmgr.h cnitem.h cnitemgroup.h \
	connector.h conrouter.h debugmanager.h docmanager.h docmanageriface.h document.h \
	documentiface.h eventinfo.h filemetainfo.h flowcodedocument.h flowcodeview.h fpnode.h \
	icndocument.h icnview.h item.h itemdocument.h itemdocumentdata.h itemgroup.h \
	iteminterface.h itemlibrary.h itemview.h katemdi.h ktechlab.h ktlcanvas.h libraryitem.h \
	microsettings.h node.h nodegroup.h oscilloscopedata.h picitem.h projectmanager.h \
	property.h recentfilesaction.h resizeoverlay.h simulator.h textdocument.h \
	textview.h utils.h variablelabel.h view.h viewcontainer.h viewiface.h


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
rc_DATA = ktechlabui.rc ktechlabcircuitui.rc \
	ktechlabflowcodeui.rc ktechlabitemviewui.rc ktechlabmechanicsui.rc ktechlabtextui.rc ktechlabkateui.rc
 
SUBDIRS = contexthelp core drawparts electronics flowparts gui languages math \
	mechanics micro

#iconsdir = $(kde_datadir)/ktechlab/icons


mimedir = $(kde_mimedir)/application
mime_DATA = x-circuit.desktop x-flowcode.desktop x-ktechlab.desktop x-microbe.desktop
EXTRA_DIST = $(mime_DATA)

katesyntaxdir = $(kde_datadir)/katepart/syntax
katesyntax_DATA = microbe.xml

_SOURCES = canvastip.h.cpp
_SOURCES = canvastip.h.cpp connectorline.h

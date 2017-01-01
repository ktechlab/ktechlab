add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(flowparts)
add_subdirectory(micro)
add_subdirectory(mechanics)
add_subdirectory(electronics)
add_subdirectory(languages)
add_subdirectory(drawparts)
add_subdirectory(contexthelp)
add_subdirectory(math)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/drawparts
    ${CMAKE_SOURCE_DIR}/src/electronics
    ${CMAKE_SOURCE_DIR}/src/electronics/components
    ${CMAKE_SOURCE_DIR}/src/electronics/simulation
    ${CMAKE_SOURCE_DIR}/src/flowparts
    ${CMAKE_SOURCE_DIR}/src/gui
    ${CMAKE_SOURCE_DIR}/src/gui/itemeditor
    ${CMAKE_SOURCE_DIR}/src/languages
    ${CMAKE_SOURCE_DIR}/src/mechanics
    ${CMAKE_SOURCE_DIR}/src/micro
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${KDE4_INCLUDE_DIR}
    ${QT_INCLUDE_DIR} )


########### next target ###############

SET(ktechlab_SRCS
   dcop_stub.cpp
   ktechlab.cpp
   node.cpp
   connector.cpp
   itemlibrary.cpp
   libraryitem.cpp
   projectmanager.cpp
   picitem.cpp
   variant.cpp
   canvasitemparts.cpp
   flowcontainer.cpp
   microsettings.cpp
   fpnode.cpp
   cells.cpp
   asmformatter.cpp
   conrouter.cpp
   nodegroup.cpp
   canvasmanipulator.cpp
   iteminterface.cpp
   itemgroup.cpp
   ciwidgetmgr.cpp
   filemetainfo.cpp
   resizeoverlay.cpp
   document.cpp
   view.cpp
   docmanager.cpp
   cnitem.cpp
   item.cpp
   cnitemgroup.cpp
   itemview.cpp
   itemdocument.cpp
   textview.cpp
   textdocument.cpp
#   circuitdocument.cpp
   flowcodedocument.cpp
   icnview.cpp
   icndocument.cpp
   viewcontainer.cpp
   circuitview.cpp
   flowcodeview.cpp
   eventinfo.cpp
   oscilloscopedata.cpp
   itemdocumentdata.cpp
   docmanageriface.cpp
   documentiface.cpp
   viewiface.cpp
   simulator.cpp
   katemdi.cpp
   debugmanager.cpp
   recentfilesaction.cpp
   variablelabel.cpp
   canvas.cpp
)

qt4_automoc(${ktechlab_SRCS} microsettings.h )

set(ktechlab_DCOP_SKEL_SRCS
    docmanageriface.h
    viewiface.h
    documentiface.h
)

#kde3_add_dcop_skels(ktechlab_SRCS ${ktechlab_DCOP_SKEL_SRCS})

add_executable(ktechlab ${ktechlab_SRCS})

target_link_libraries( ktechlab
    core gui micro flowparts
	mechanics electronics elements components languages drawparts
	itemeditor math
	${KDE4_KHTML_LIBRARY} # khtml
	${KDE4_KUTILS_LIBRARY} # kutils
	${GPSIM_LIBRARY}
	${KDE4_KTEXTEDITOR_LIBRARY} # ktexteditor
	${KDE4_KIO_LIBRARY} # kio
	${QT_AND_KDECORE_LIBS}
	${KDE4_KPARTS_LIBRARY} # kparts
	# kdeprint # removed in kde4, use Qt4
	${QT_QTXML_LIBRARY}
	${KDE4_KDEUI_LIBRARY} # kdeui
	${QT_QTGUI_LIBRARY} # QtGui
	${KDE4_KDECORE_LIBRARY} # kdecore
	${KDE4_KDE3SUPPORT_LIBRARY} # kde3support
	${QT_QT3SUPPORT_LIBRARY} # Qt3Support
	${GPSIM_LIBRARY} # gpsim
	${QT_QTCORE_LIBRARY} # QtCore
	${KDE4_KFILE_LIBRARY} # kfile
	)

install(TARGETS ktechlab DESTINATION bin)


########### install files ###############

install(FILES  microbe.xml DESTINATION share/apps/katepart/syntax)
install(FILES  ktechlabui.rc ktechlabcircuitui.rc  	ktechlabflowcodeui.rc
	ktechlabitemviewui.rc ktechlabmechanicsui.rc ktechlabtextui.rc
	ktechlabkateui.rc DESTINATION share/apps/ktechlab)
install(FILES  ktechlab.desktop DESTINATION share/applnk/Development)
install(FILES ktechlab_mime.xml DESTINATION ${XDG_MIME_INSTALL_DIR})



#original Makefile.am contents follow:

### Makefile.am for ktechlab
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#bin_PROGRAMS = ktechlab
#
## set the include path for X, qt and KDE
#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/drawparts \
#	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components \
#	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui \
#	-I$(top_srcdir)/src/gui/itemeditor -I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics \
#	-I$(top_srcdir)/src/micro $(all_includes)
#
## the library search path. 
#ktechlab_LDFLAGS = -module $(all_libraries) $(KDE_RPATH)
#
## the libraries to link against.
#ktechlab_LDADD = $(top_builddir)/src/gui/libgui.la \
#	$(top_builddir)/src/micro/libmicro.la $(top_builddir)/src/flowparts/libflowparts.la \
#	$(top_builddir)/src/mechanics/libmechanics.la $(top_builddir)/src/electronics/libelectronics.la \
#	$(top_builddir)/src/electronics/simulation/libelements.la $(top_builddir)/src/electronics/components/libcomponents.la \
#	$(top_builddir)/src/languages/liblanguages.la $(top_builddir)/src/drawparts/libdrawparts.la \
#	$(top_builddir)/src/core/libcore.la $(top_builddir)/src/gui/itemeditor/libitemeditor.la \
#	$(top_builddir)/src/math/libmath.la \
#	-lkhtml -lkutils $(LIB_GPSIM) -lktexteditor $(LIB_KFILE) $(LIB_KDEPRINT)
#
## which sources should be compiled for ktechlab
#ktechlab_SOURCES = ktechlab.cpp node.cpp connector.cpp itemlibrary.cpp \
#				libraryitem.cpp projectmanager.cpp picitem.cpp variant.cpp canvasitemparts.cpp \
#				flowcontainer.cpp microsettings.cpp fpnode.cpp cells.cpp asmformatter.cpp conrouter.cpp \
#				nodegroup.cpp canvasmanipulator.cpp iteminterface.cpp itemgroup.cpp ciwidgetmgr.cpp \
#				filemetainfo.cpp resizeoverlay.cpp document.cpp view.cpp docmanager.cpp cnitem.cpp \
#				item.cpp cnitemgroup.cpp itemview.cpp itemdocument.cpp textview.cpp \
#				textdocument.cpp circuitdocument.cpp flowcodedocument.cpp icnview.cpp icndocument.cpp \
#				viewcontainer.cpp circuitview.cpp flowcodeview.cpp eventinfo.cpp oscilloscopedata.cpp \
#				itemdocumentdata.cpp docmanageriface.cpp documentiface.cpp viewiface.cpp \
#				docmanageriface.skel viewiface.skel documentiface.skel simulator.cpp katemdi.cpp \
#				debugmanager.cpp recentfilesaction.cpp variablelabel.cpp canvas.cpp
#
#ktechlab_PCH = AUTO
#
#
## these are the headers for your project
#noinst_HEADERS = ktechlab.h node.h connector.h itemlibrary.h libraryitem.h \
#		projectmanager.h picitem.h canvasitemparts.h microsettings.h fpnode.h cells.h \
#		asmformatter.h conrouter.h nodegroup.h canvasmanipulator.h iteminterface.h itemgroup.h \
#		ciwidgetmgr.h filemetainfo.h resizeoverlay.h document.h view.h docmanager.h cnitem.h \
#		item.h cnitemgroup.h itemview.h itemdocument.h textview.h textdocument.h \
#		circuitdocument.h flowcodedocument.h icnview.h icndocument.h viewcontainer.h \
#		circuitview.h flowcodeview.h eventinfo.h oscilloscopedata.h itemdocumentdata.h \
#		docmanageriface.h documentiface.h viewiface.h simulator.h katemdi.h debugmanager.h \
#		recentfilesaction.h variablelabel.h utils.h property.h canvas.h
#
#
## client stuff
#
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui" -o -name "*.kcfg"` > rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#		$(XGETTEXT) $$LIST -o $(podir)/ktechlab.pot; \
#	fi
#
##KDE_ICON = ktechlab
##KDE_ICON = AUTO
#
## this is where the kdelnk file will go 
#kdelnkdir = $(kde_appsdir)/Development
#kdelnk_DATA = ktechlab.desktop
#
## this is where the XML-GUI resource file goes
#rcdir = $(kde_datadir)/ktechlab
#rc_DATA = ktechlabui.rc ktechlabcircuitui.rc \
#	ktechlabflowcodeui.rc ktechlabitemviewui.rc ktechlabmechanicsui.rc ktechlabtextui.rc ktechlabkateui.rc
# 
#SUBDIRS = core gui flowparts micro mechanics electronics languages drawparts \
#	contexthelp math
##iconsdir = $(kde_datadir)/ktechlab/icons
#
#
#mimedir = $(kde_mimedir)/application
#mime_DATA = x-circuit.desktop x-flowcode.desktop x-ktechlab.desktop x-microbe.desktop
#EXTRA_DIST = $(mime_DATA)
#
#katesyntaxdir = $(kde_datadir)/katepart/syntax
#katesyntax_DATA = microbe.xml
#

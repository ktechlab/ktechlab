include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}  # needed for subdirs
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_BINARY_DIR}/core  # for the kcfg file
    ${CMAKE_CURRENT_SOURCE_DIR}/drawparts
    ${CMAKE_CURRENT_SOURCE_DIR}/electronics
    ${CMAKE_CURRENT_SOURCE_DIR}/electronics/components
    ${CMAKE_CURRENT_SOURCE_DIR}/electronics/simulation
    ${CMAKE_CURRENT_SOURCE_DIR}/flowparts
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
    ${CMAKE_CURRENT_BINARY_DIR}/gui  # for ui-generated files
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/itemeditor
    ${CMAKE_CURRENT_SOURCE_DIR}/languages
    ${CMAKE_CURRENT_SOURCE_DIR}/mechanics
    ${CMAKE_CURRENT_SOURCE_DIR}/micro
    ${CMAKE_CURRENT_SOURCE_DIR}/ktlqt3support
    ${KDE4_INCLUDES}
    ${QT_INCLUDES})
if(GPSim_FOUND)
    include_directories(${GPSim_INCLUDE_DIRS})
    set(CMAKE_CXX_FLAGS ${KDE4_ENABLE_EXCEPTIONS})
endif()

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
add_subdirectory(ktlqt3support)

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
   canvasitems.cpp
   canvasitemlist.cpp
)

set(ktechlab_DCOP_SKEL_SRCS
    docmanageriface.h
    viewiface.h
    documentiface.h
)

#kde3_add_dcop_skels(ktechlab_SRCS ${ktechlab_DCOP_SKEL_SRCS})

kde4_add_executable(ktechlab ${ktechlab_SRCS})

target_link_libraries( ktechlab
    core gui micro flowparts
    ktlqt3support
	mechanics electronics elements components languages drawparts
	itemeditor math
	${KDE4_KHTML_LIBRARY} # khtml
	${GPSIM_LIBRARY}
	${KDE4_KTEXTEDITOR_LIBRARY} # ktexteditor
	${KDE4_KIO_LIBRARY} # kio
	${KDE4_KPARTS_LIBRARY} # kparts
	${QT_QTXML_LIBRARY}
	${KDE4_KDEUI_LIBRARY} # kdeui
	${QT_QTGUI_LIBRARY} # QtGui
	${KDE4_KDECORE_LIBRARY} # kdecore
	#${KDE4_KDE3SUPPORT_LIBRARY} # kde3support # 2016.06.02 - should not be needed
	${QT_QT3SUPPORT_LIBRARY} # Qt3Support
	${QT_QTCORE_LIBRARY} # QtCore
	${KDE4_KFILE_LIBRARY} # kfile
	)
if(GPSim_FOUND)
    target_link_libraries(ktechlab ${GPSim_LIBRARIES})
endif()

install(TARGETS ktechlab ${INSTALL_TARGETS_DEFAULT_ARGS})

# for helping testing

kde4_add_library(test_ktechlab STATIC ${ktechlab_SRCS})


########### install files ###############

install(FILES microbe.xml DESTINATION ${DATA_INSTALL_DIR}/katepart/syntax)
install(FILES  ktechlabui.rc ktechlabcircuitui.rc  	ktechlabflowcodeui.rc
	ktechlabitemviewui.rc ktechlabmechanicsui.rc ktechlabtextui.rc
	ktechlabkateui.rc DESTINATION ${DATA_INSTALL_DIR}/ktechlab)
install(FILES org.kde.ktechlab.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ktechlab_mime.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
install(FILES org.kde.ktechlab.appdata.xml DESTINATION ${SHARE_INSTALL_PREFIX}/metainfo )

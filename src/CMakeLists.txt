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
)
if(GPSim_FOUND)
    # mark SYSTEM to have the compiler not emit warnings for those headers
    include_directories(SYSTEM ${GPSim_INCLUDE_DIRS})
    kde_enable_exceptions()
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

set(core_KCFG_SRCS
    core/ktlconfig.kcfgc
)
kconfig_add_kcfg_files(core_STAT_SRCS ${core_KCFG_SRCS})

set(gui_UI
    ./gui/gpasmsettingswidget.ui
    ./gui/newpinmappingwidget.ui
    ./gui/outputmethodwidget.ui
    ./gui/logicwidget.ui
    ./gui/microsettingswidget.ui
    ./gui/createsubprojectwidget.ui
    ./gui/picprogrammerconfigwidget.ui
    ./gui/contexthelpwidget.ui
    ./gui/linkeroptionswidget.ui
    ./gui/programmerwidget.ui
    ./gui/newprojectwidget.ui
    ./gui/scopescreenwidget.ui
    ./gui/oscilloscopewidget.ui
    ./gui/newfilewidget.ui
    ./gui/processingoptionswidget.ui
    ./gui/asmformattingwidget.ui
    ./gui/generaloptionswidget.ui
    ./gui/sdccoptionswidget.ui
    ./gui/gplinksettingswidget.ui
)

ki18n_wrap_ui(gui_STAT_SRCS ${gui_UI})

SET(ktechlab_SRCS
    ${core_STAT_SRCS}
    ${gui_STAT_SRCS}
    ./document.cpp
    ./gui/programmerdlg.cpp
    ./gui/itemeditor/propertyeditoritem.cpp
    ./gui/itemeditor/itemeditor.cpp
    ./gui/itemeditor/propertyeditor.cpp
    ./gui/itemeditor/propertysubeditor.cpp
    ./gui/itemeditor/componentmodelwidget.cpp
    ./gui/itemeditor/propertyeditorlist.cpp
    ./gui/itemeditor/propertyeditorcolor.cpp
    ./gui/itemeditor/orientationwidget.cpp
    ./gui/itemeditor/propertyeditorinput.cpp
    ./gui/itemeditor/propertyeditorfile.cpp
    ./gui/probepositioner.cpp
    ./gui/richtexteditor.cpp
    ./gui/logview.cpp
    ./gui/contexthelp.cpp
    ./gui/oscilloscopeview.cpp
    ./gui/doublespinbox.cpp
    ./gui/lineedit.cpp
    ./gui/inputdialog.cpp
    ./gui/microselectwidget.cpp
    ./gui/scopescreen.cpp
    ./gui/itemselector.cpp
    ./gui/scopescreenview.cpp
    ./gui/microsettingsdlg.cpp
    ./gui/colorcombo.cpp
    ./gui/symbolviewer.cpp
    ./gui/oscilloscope.cpp
    ./gui/newfiledlg.cpp
    ./gui/projectdlgs.cpp
    ./gui/outputmethoddlg.cpp
    ./gui/scopeviewbase.cpp
    ./gui/settingsdlg.cpp
    ./gui/imageexportdlg.cpp
    ./projectmanager.cpp
    ./cells.cpp
    ./docmanageriface.cpp
    ./micro/picinfo16bit.cpp
    ./micro/picinfo12bit.cpp
    ./micro/picinfo.cpp
    ./micro/micropackage.cpp
    ./micro/microlibrary.cpp
    ./micro/picinfo14bit.cpp
    ./micro/microinfo.cpp
    ./micro/asminfo.cpp
    ./ciwidgetmgr.cpp
    ./fpnode.cpp
    ./textdocument.cpp
    ./filemetainfo.cpp
    ./resizeoverlay.cpp
    ./viewcontainer.cpp
    ./microsettings.cpp
    ./nodegroup.cpp
    ./flowcodeview.cpp
    ./debugmanager.cpp
    ./flowparts/readport.cpp
    ./flowparts/inputflownode.cpp
    ./flowparts/repeat.cpp
    ./flowparts/embed.cpp
    ./flowparts/sub.cpp
    ./flowparts/while.cpp
    ./flowparts/flowconnectorlist.cpp
    ./flowparts/setpin.cpp
    ./flowparts/pinmapping.cpp
    ./flowparts/interrupt.cpp
    ./flowparts/inputbutton.cpp
    ./flowparts/flowicndocument.cpp
    ./flowparts/testpin.cpp
    ./flowparts/flowconnector.cpp
    ./flowparts/writeport.cpp
    ./flowparts/flowpart.cpp
    ./flowparts/forloop.cpp
    ./flowparts/count.cpp
    ./flowparts/outputflownode.cpp
    ./flowparts/end.cpp
    ./flowparts/callsub.cpp
    ./flowparts/unary.cpp
    ./flowparts/start.cpp
    ./flowparts/varassignment.cpp
    ./flowparts/delay.cpp
    ./flowparts/pulse.cpp
    ./flowparts/junctionflownode.cpp
    ./flowparts/keypad.cpp
    ./flowparts/sevenseg.cpp
    ./flowparts/varcomparison.cpp
    ./math/qvector.cpp
    ./math/qmatrix.cpp
    ./picitem.cpp
#     ./core/main.cpp
    ./core/diagnosticstyle.cpp
    ./core/logtofilemsghandler.cpp
    ./node.cpp
    ./documentiface.cpp
    ./docmanager.cpp
    ./itemview.cpp
    ./viewiface.cpp
    ./icndocument.cpp
    ./itemdocument.cpp
    ./itemgroup.cpp
    ./cnitemgroup.cpp
    ./canvasmanipulator.cpp
    ./iteminterface.cpp
    ./mechanics/mechanicsitem.cpp
    ./mechanics/mechanicsdocument.cpp
    ./mechanics/chassiscircular2.cpp
    ./mechanics/mechanicssimulation.cpp
    ./mechanics/mechanicsview.cpp
    ./mechanics/mechanicsgroup.cpp
    ./electronics/electronicconnector.cpp
    ./electronics/circuitdocument.cpp
    ./electronics/ecnode.cpp
#     ./electronics/models/utils/spice-to-nice.cpp
    ./electronics/pinnode.cpp
    ./electronics/port.cpp
    ./electronics/components/variablecapacitor.cpp
    ./electronics/components/rotoswitch.cpp
    ./electronics/components/led.cpp
    ./electronics/components/binarycounter.cpp
    ./electronics/components/probe.cpp
    ./electronics/components/inductor.cpp
    ./electronics/components/eccurrentsignal.cpp
    ./electronics/components/ecpotentiometer.cpp
    ./electronics/components/flipflop.cpp
    ./electronics/components/serialportcomponent.cpp
    ./electronics/components/piccomponentpin.cpp
    ./electronics/components/ecopamp.cpp
    ./electronics/components/multiplexer.cpp
    ./electronics/components/ecmosfet.cpp
    ./electronics/components/dependentsource.cpp
    ./electronics/components/ecground.cpp
    ./electronics/components/resistordip.cpp
    ./electronics/components/magnitudecomparator.cpp
    ./electronics/components/piccomponent.cpp
    ./electronics/components/matrixdisplay.cpp
    ./electronics/components/eccurrentsource.cpp
    ./electronics/components/ecvoltagesignal.cpp
    ./electronics/components/demultiplexer.cpp
    ./electronics/components/externalconnection.cpp
    ./electronics/components/ledbargraphdisplay.cpp
    ./electronics/components/bidirled.cpp
    ./electronics/components/ecbcdto7segment.cpp
    ./electronics/components/ecsevensegment.cpp
    ./electronics/components/ecvoltagesource.cpp
    ./electronics/components/parallelportcomponent.cpp
    ./electronics/components/ec555.cpp
    ./electronics/components/resistor.cpp
    ./electronics/components/voltageregulator.cpp
    ./electronics/components/multiinputgate.cpp
    ./electronics/components/discretelogic.cpp
    ./electronics/components/pushswitch.cpp
    ./electronics/components/ecjfet.cpp
    ./electronics/components/meter.cpp
    ./electronics/components/variableresistor.cpp
    ./electronics/components/ecsignallamp.cpp
    ./electronics/components/ecclockinput.cpp
    ./electronics/components/ecbjt.cpp
    ./electronics/components/matrixdisplaydriver.cpp
    ./electronics/components/fulladder.cpp
    ./electronics/components/ecdiode.cpp
    ./electronics/components/ecfixedvoltage.cpp
    ./electronics/components/bussplitter.cpp
    ./electronics/components/capacitor.cpp
    ./electronics/components/ram.cpp
    ./electronics/components/toggleswitch.cpp
    ./electronics/components/ecsubcircuit.cpp
    ./electronics/components/addac.cpp
    ./electronics/components/eckeypad.cpp
    ./electronics/junctionnode.cpp
    ./electronics/switch.cpp
    ./electronics/gpsimprocessor.cpp
    ./electronics/wire.cpp
    ./electronics/componentmodellibrary.cpp
    ./electronics/pin.cpp
    ./electronics/circuiticndocument.cpp
    ./electronics/simulation/circuit.cpp
    ./electronics/simulation/element.cpp
    ./electronics/simulation/matrix.cpp
    ./electronics/simulation/diode.cpp
    ./electronics/simulation/elementsignal.cpp
    ./electronics/simulation/voltagesignal.cpp
    ./electronics/simulation/capacitance.cpp
    ./electronics/simulation/resistance.cpp
    ./electronics/simulation/elementset.cpp
    ./electronics/simulation/inductance.cpp
    ./electronics/simulation/voltagesource.cpp
    ./electronics/simulation/voltagepoint.cpp
    ./electronics/simulation/vcvs.cpp
    ./electronics/simulation/mosfet.cpp
    ./electronics/simulation/jfet.cpp
    ./electronics/simulation/ccvs.cpp
    ./electronics/simulation/bjt.cpp
    ./electronics/simulation/opamp.cpp
    ./electronics/simulation/currentsource.cpp
    ./electronics/simulation/cccs.cpp
    ./electronics/simulation/vccs.cpp
    ./electronics/simulation/logic.cpp
    ./electronics/simulation/nonlinear.cpp
    ./electronics/simulation/currentsignal.cpp
    ./electronics/simulation/reactive.cpp
    ./electronics/subcircuits.cpp
    ./electronics/component.cpp
    ./circuitview.cpp
    ./itemlibrary.cpp
    ./simulator.cpp
    ./dcop_stub.cpp
    ./variablelabel.cpp
    ./item.cpp
    ./itemdocumentdata.cpp
    ./ktlqt3support/ktlfindqobjectchild.cpp
    ./ktlqt3support/ktlq3scrollview.cpp
    ./ktlqt3support/ktlq3polygonscanner.cpp
    ./ktlqt3support/ktlq3frame.cpp
    ./oscilloscopedata.cpp
    ./variant.cpp
    ./connector.cpp
    ./icnview.cpp
    ./drawparts/drawpart.cpp
    ./drawparts/dpline.cpp
    ./drawparts/solidshape.cpp
    ./drawparts/dptext.cpp
    ./drawparts/dpimage.cpp
    ./textview.cpp
    ./katemdi.cpp
    ./view.cpp
    ./ktechlab.cpp
    ./canvasitemparts.cpp
    ./eventinfo.cpp
    ./canvasitemlist.cpp
    ./asmformatter.cpp
    ./cnitem.cpp
    ./flowcodedocument.cpp
    ./libraryitem.cpp
    ./conrouter.cpp
    ./recentfilesaction.cpp
    ./canvas.cpp
    ./canvasitems.cpp
    ./flowcontainer.cpp
    ./languages/processchain.cpp
    ./languages/gpdasm.cpp
    ./languages/gplink.cpp
    ./languages/gplib.cpp
    ./languages/language.cpp
    ./languages/sourceline.cpp
    ./languages/microbe.cpp
    ./languages/picprogrammer.cpp
    ./languages/externallanguage.cpp
    ./languages/asmparser.cpp
    ./languages/languagemanager.cpp
    ./languages/gpasm.cpp
    ./languages/sdcc.cpp
    ./languages/flowcode.cpp
)

set(ktechlab_DCOP_SKEL_SRCS
    docmanageriface.h
    viewiface.h
    documentiface.h
)

#kde3_add_dcop_skels(ktechlab_SRCS ${ktechlab_DCOP_SKEL_SRCS})

add_executable(ktechlab
    core/main.cpp
    ${ktechlab_SRCS}
    )

target_link_libraries( ktechlab
    #core 
#     gui 
    #micro 
    #flowparts
    #ktlqt3support
	#mechanics electronics elements components languages drawparts
	#itemeditor math
    KF5::TextEditor
    KF5::Parts
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::IconThemes
    KF5::KIOCore
    KF5::KHtml
    KF5::XmlGui
    KF5::WidgetsAddons
    KF5::WindowSystem

    Qt5::Widgets
    Qt5::PrintSupport
    Qt5::SerialPort
)
if(GPSim_FOUND)
    target_link_libraries(ktechlab ${GPSim_LIBRARIES})
endif()

install(TARGETS ktechlab ${INSTALL_TARGETS_DEFAULT_ARGS})


# message(STATUS "include_dir begin")
# get_property(dirs TARGET ktechlab PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "ktechlab include_dir='${dir}'")
# endforeach()
# message(STATUS "include_dir end")

# message(STATUS "include_dir begin")
# get_property(dirs TARGET KF5::TextEditor PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "KF5::TextEditor include_dir='${dir}'")
# endforeach()
# message(STATUS "include_dir end")

# for helping testing

if(BUILD_TESTING)

    add_library(test_ktechlab STATIC ${ktechlab_SRCS})

    target_link_libraries(test_ktechlab
        KF5::TextEditor
        KF5::IconThemes
        KF5::Parts
        KF5::KHtml
        KF5::WidgetsAddons
        KF5::WindowSystem

        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::SerialPort
    )

endif()

########### install files ###############

install(FILES microbe.xml DESTINATION ${DATA_INSTALL_DIR}/katepart5/syntax)
install(FILES
    ktechlabui.rc
    ktechlabcircuitui.rc
    ktechlabflowcodeui.rc
	ktechlabitemviewui.rc
	ktechlabmechanicsui.rc
	ktechlabtextui.rc
	ktechlabkateui.rc
	DESTINATION ${KXMLGUI_INSTALL_DIR}/ktechlab)
install(FILES org.kde.ktechlab.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES ktechlab_mime.xml DESTINATION ${XDG_MIME_INSTALL_DIR})
install(FILES org.kde.ktechlab.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR} )

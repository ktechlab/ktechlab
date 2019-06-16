add_subdirectory(itemeditor)

########### next target ###############

SET(gui_STAT_SRCS
#    contexthelp.cpp
#    doublespinbox.cpp
#    itemselector.cpp
#    microsettingsdlg.cpp
#    newfiledlg.cpp
#    outputmethoddlg.cpp
#    settingsdlg.cpp
#    oscilloscope.cpp
#    oscilloscopeview.cpp
#    probepositioner.cpp
#    logview.cpp
#    projectdlgs.cpp
#    microselectwidget.cpp
#    symbolviewer.cpp
#    programmerdlg.cpp
#    colorcombo.cpp
#    scopescreenview.cpp
#    scopescreen.cpp
#    scopeviewbase.cpp
#    richtexteditor.cpp
#    lineedit.cpp
)

set(gui_UI
#     asmformattingwidget.ui
#     gpasmsettingswidget.ui
#     logicwidget.ui
#     newfilewidget.ui
#     newprojectwidget.ui
#     outputmethodwidget.ui
#     microsettingswidget.ui
#     oscilloscopewidget.ui
#     generaloptionswidget.ui
#     createsubprojectwidget.ui
#     processingoptionswidget.ui
#     sdccoptionswidget.ui
#     linkeroptionswidget.ui
#     picprogrammerconfigwidget.ui
#     newpinmappingwidget.ui
#     programmerwidget.ui
#     contexthelpwidget.ui
#     scopescreenwidget.ui
#     gplinksettingswidget.ui
)

#kde4_add_ui_files(gui_STAT_SRCS ${gui_UI})
ki18n_wrap_ui(gui_STAT_SRCS ${gui_UI})

add_library(gui STATIC ${gui_STAT_SRCS})
target_link_libraries( gui itemeditor )

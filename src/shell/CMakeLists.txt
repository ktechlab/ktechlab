add_subdirectory( settings )

include_directories( ${KDEVPLATFORM_INCLUDE_DIR} )
set(ktlshell_LIB_SRCS
    core.cpp
    uicontroller.cpp
    plugincontroller.cpp
    shellextension.cpp
    mainwindow.cpp
    mainwindow_p.cpp
    documentcontroller.cpp
    partcontroller.cpp
    projectcontroller.cpp
    project.cpp
    savedialog.cpp
    session.cpp
    sessiondialog.cpp
    sessioncontroller.cpp
    textdocument.cpp
    statusbar.cpp
    partdocument.cpp
    openprojectdialog.cpp
    openprojectpage.cpp
    projectinfopage.cpp
    languagecontroller.cpp
    language.cpp
    completionsettings.cpp
    sourceformattercontroller.cpp
)

kde4_add_ui_files( ktlshell_LIB_SRCS
   sessiondialog.ui
   projectinfopage.ui
)
kde4_add_library( ktlshell SHARED ${ktlshell_LIB_SRCS} )

target_link_libraries(ktlshell
    ${KDEVPLATFORM_OUTPUTVIEW_LIBRARY}
    ${KDEVPLATFORM_PROJECT_LIBRARY}
    ${KDEVPLATFORM_LANGUAGE_LIBRARY}
    ${KDEVPLATFORM_INTERFACES_LIBRARY}
    ${KDEVPLATFORM_SUBLIME_LIBRARY}
    ${KDE4_KNOTIFYCONFIG_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KFILE_LIBS}
    ${KDE4_KUTILS_LIBS}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}

    ${KDEVPLATFORM_SHELL_LIBRARY}
)

#set_target_properties(ktlshell PROPERTIES VERSION )
install( TARGETS ktlshell ${INSTALL_TARGETS_DEFAULT_ARGS} )

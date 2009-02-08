
set(kdevplatform_SRCS
   mainwindow.cpp
   mainwindow_p.cpp
   plugincontroller.cpp
   shellextension.cpp
   core.cpp
   uicontroller.cpp
   projectcontroller.cpp
   project.cpp
   partcontroller.cpp
   #document.cpp
   partdocument.cpp
   textdocument.cpp
   documentcontroller.cpp
   languagecontroller.cpp
   language.cpp
   statusbar.cpp
   runcontroller.cpp
   sessioncontroller.cpp
   session.cpp
   savedialog.cpp
   sessiondialog.cpp
   sourceformattercontroller.cpp
   completionsettings.cpp
   openprojectpage.cpp
   openprojectdialog.cpp
   projectinfopage.cpp
   selectioncontroller.cpp
)

set(ktlshell_LIB_SRCS
    ktechlabcore.cpp
    ${kdevplatform_SRCS}
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

    #    ${KDEVPLATFORM_SHELL_LIBRARY}
)

#set_target_properties(ktlshell PROPERTIES VERSION )
install( TARGETS ktlshell ${INSTALL_TARGETS_DEFAULT_ARGS} )

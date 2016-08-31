
add_executable(test_loaded_icons test-loaded-icons.cpp)

target_link_libraries( test_loaded_icons
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

add_executable(test_loaded_icons test-loaded-icons.cpp)

target_link_libraries( test_loaded_icons
    ${KDE4_KDEUI_LIBRARY} # kdeui
    ${QT_QTGUI_LIBRARY} # QtGui
    ${KDE4_KDECORE_LIBRARY} # kdecore
    ${QT_QTCORE_LIBRARY} # QtCore
    )

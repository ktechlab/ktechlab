add_executable(test_loaded_icons test-loaded-icons.cpp)

target_link_libraries( test_loaded_icons
    KF5::I18n
    KF5::KDELibs4Support
    ${KDE4_KDEUI_LIBRARY} # kdeui
    ${QT_QTGUI_LIBRARY} # QtGui
    ${KDE4_KDECORE_LIBRARY} # kdecore
    ${QT_QTCORE_LIBRARY} # QtCore
    )

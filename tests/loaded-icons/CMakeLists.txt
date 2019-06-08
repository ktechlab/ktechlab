
add_executable(test_loaded_icons test-loaded-icons.cpp)

target_link_libraries( test_loaded_icons
    ${QT_QTGUI_LIBRARY} # QtGui
    ${QT_QTCORE_LIBRARY} # QtCore
    
    Qt5::Widgets
    KF5::CoreAddons
    KF5::I18n
    KF5::IconThemes
    KF5::KDELibs4Support
    )

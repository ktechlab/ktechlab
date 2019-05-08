
add_executable(test_loaded_icons test-loaded-icons.cpp)

target_link_libraries( test_loaded_icons
    ${QT_QTGUI_LIBRARY} # QtGui
    ${QT_QTCORE_LIBRARY} # QtCore
    KF5::I18n
    KF5::CoreAddons
    KF5::IconThemes
    )

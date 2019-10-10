
add_executable(test_loaded_icons test-loaded-icons.cpp)

# https://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake/20165220#20165220
set_property(TARGET test_loaded_icons PROPERTY CXX_STANDARD 11)
set_property(TARGET test_loaded_icons PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries( test_loaded_icons
    ${QT_QTGUI_LIBRARY} # QtGui
    ${QT_QTCORE_LIBRARY} # QtCore
    
    Qt5::Widgets
    KF5::CoreAddons
    KF5::I18n
    KF5::IconThemes
    KF5::KDELibs4Support
    )

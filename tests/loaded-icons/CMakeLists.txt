
add_executable(test_loaded_icons test-loaded-icons.cpp)

target_link_libraries( test_loaded_icons
    KF5::CoreAddons
    KF5::I18n
    KF5::IconThemes
    Qt5::Widgets
)

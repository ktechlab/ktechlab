
add_executable(test_loaded_icons test-loaded-icons.cpp)

target_link_libraries( test_loaded_icons
    KF6::CoreAddons
    KF6::I18n
    KF6::IconThemes
    Qt6::Widgets
)

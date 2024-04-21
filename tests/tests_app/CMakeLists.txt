
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)

include_directories(
    ${SRC_DIR}  # needed for subdirs
    ${SRC_DIR}/core
    ${CMAKE_BINARY_DIR}/src/core  # for the kcfg file
    ${SRC_DIR}/drawparts
    ${SRC_DIR}/electronics
    ${SRC_DIR}/electronics/components
    ${SRC_DIR}/electronics/simulation
    ${SRC_DIR}/flowparts
    ${SRC_DIR}/gui
    ${CMAKE_BINARY_DIR}/src/gui  # for ui-generated files
    ${SRC_DIR}/gui/itemeditor
    ${SRC_DIR}/languages
    ${SRC_DIR}/mechanics
    ${SRC_DIR}/micro
)
if(GPSim_FOUND)
    include_directories(SYSTEM ${GPSim_INCLUDE_DIRS})
    kde_enable_exceptions()
endif()

add_executable(tests_app tests_app.cpp)

target_link_libraries( tests_app
    test_ktechlab
#     ktlqt3support
    #core 
#     gui micro flowparts
#     mechanics electronics elements components languages drawparts
#     itemeditor
#     test_ktechlab
#     math
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::KIOCore
    KF6::KIOWidgets
    KF6::CoreAddons
    KF6::XmlGui
    KF6::TextEditor

    Qt6::Widgets
    Qt6::Test
    Qt6::Core5Compat
)
if(GPSim_FOUND)
    target_link_libraries(tests_app ${GPSim_LIBRARIES})
endif()

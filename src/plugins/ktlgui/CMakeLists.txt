
include_directories(
${CMAKE_SOURCE_DIR}/src
)

set( ktlgui_PART_SRCS
  ktlguiplugin.cpp
  newfiledlg.cpp
)

kde4_add_ui_files(ktlgui_PART_SRCS newfilewidget.ui)

kde4_add_plugin(ktlgui ${ktlgui_PART_SRCS})

target_link_libraries(ktlgui
 ktlinterfaces
 ${KDEVPLATFORM_INTERFACES_LIBRARIES}
)

install(TARGETS ktlgui DESTINATION ${PLUGIN_INSTALL_DIR} )

ktl_install_desktop( ktlgui.desktop )

install( FILES ktechlabui.rc DESTINATION ${DATA_INSTALL_DIR}/ktlgui )

include_directories(
${CMAKE_SOURCE_DIR}/src
)

set( ktlgui_PART_SRCS
  ktlguiplugin.cpp
)

kde4_add_plugin(ktlgui ${ktlgui_PART_SRCS})

target_link_libraries(ktlgui
 ktlinterfaces
 ${KDEVPLATFORM_INTERFACES_LIBRARIES}
)

install(TARGETS ktlgui DESTINATION ${PLUGIN_INSTALL_DIR} )

ktl_install_desktop( ktlgui.desktop )


include_directories(
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${KDEVPLATFORM_INCLUDE_DIR}
   )

set( plugincontroller_SRCS
   plugincontroller.cpp
   )

kde4_add_plugin(ktlplugincontroller ${plugincontroller_SRCS})

target_link_libraries( ktlplugincontroller
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   )

install(TARGETS ktlplugincontroller DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES
   ktlplugincontroller.desktop

   DESTINATION ${SERVICES_INSTALL_DIR} )

include_directories(
  ${KDEVPLATFORM_INCLUDE_DIR}
)

set( ktlproject_SRCS
  ktlprojectmanager.cpp
)

kde4_add_plugin(ktlproject ${ktlproject_SRCS})

target_link_libraries( ktlproject
  ${KDEVPLATFORM_INTERFACES_LIBRARIES}
  ${KDEVPLATFORM_PROJECT_LIBRARIES}
)

install( TARGETS ktlproject DESTINATION ${PLUGIN_INSTALL_DIR} )

ktl_install_desktop( ktlproject.desktop )

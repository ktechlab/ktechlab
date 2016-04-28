include_directories(
  ${KDEVPLATFORM_INCLUDE_DIR}
)

set( ktlproject_SRCS
  ktlprojectmanager.cpp
)

kde4_add_plugin(ktlproject ${ktlproject_SRCS})

if (${KDEV_PLUGIN_VERSION} LESS 17)
  set(EXTRA_LIBS)
else(${KDEV_PLUGIN_VERSION} LESS 17)
  set(EXTRA_LIBS ${KDEVPLATFORM_UTIL_LIBRARIES})
endif(${KDEV_PLUGIN_VERSION} LESS 17)

target_link_libraries( ktlproject
  ${KDEVPLATFORM_INTERFACES_LIBRARIES}
  ${KDEVPLATFORM_PROJECT_LIBRARIES}
  ${EXTRA_LIBS}
)

install( TARGETS ktlproject DESTINATION ${PLUGIN_INSTALL_DIR} )

ktl_install_desktop( ktlproject.desktop )

# building of the simulator plugin

set( ktlsimulator_PART_SRCS
   simulatorplugin.cpp
)

kde4_add_plugin( ktlsimulator ${ktlsimulator_PART_SRCS} )

target_link_libraries( ktlsimulator
    ktlinterfaces
    ${KDE4_KDECORE_LIBS}
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    )

install(TARGETS ktlsimulator DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES ktlsimulator.desktop DESTINATION ${SERVICES_INSTALL_DIR} )


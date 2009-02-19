########### next target ###############

set( ktlcircuit_PART_SRCS
   ktlcircuitplugin.cpp
)

kde4_add_plugin(ktlcircuit ${ktlcircuit_PART_SRCS})
target_link_libraries(ktlcircuit ${KDE4_KDECORE_LIBS} ${KDEVPLATFORM_INTERFACES_LIBRARY} ${KDE4_KTEXTEDITOR_LIBS})

install(TARGETS ktlcircuit DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES ktlcircuit.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
#install( FILES ktlcircuit.rc DESTINATION ${DATA_INSTALL_DIR}/ktlcircuit )


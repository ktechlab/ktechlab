add_subdirectory( themes )

########### next target ###############
include_directories(
   ${CMAKE_SOURCE_DIR}/src
)

set( ktlpassive_PART_SRCS
   ktlpassiveplugin.cpp
   resistor.cpp
)

kde4_add_plugin(ktlpassive ${ktlpassive_PART_SRCS})
target_link_libraries(ktlpassive
   ${KDE4_KDECORE_LIBS}
   ${KDE4_PLASMA_LIBS}
   ktlinterfaces
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   )

install(TARGETS ktlpassive DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES ktlpassive.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
#install( FILES ktlpassive.rc DESTINATION ${DATA_INSTALL_DIR}/ktechlab/components/passive )


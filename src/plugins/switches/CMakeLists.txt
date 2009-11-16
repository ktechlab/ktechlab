add_subdirectory( themes )

########### next target ###############
include_directories(
   ${CMAKE_SOURCE_DIR}/src
)

set( ktlswitches_PART_SRCS
   ktlswitchesplugin.cpp
   rotaryswitch.cpp
)

kde4_add_plugin(ktlswitches ${ktlswitches_PART_SRCS})
target_link_libraries(ktlswitches
   ${KDE4_KDECORE_LIBS}
   ${KDE4_PLASMA_LIBS}
   ktlinterfaces
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   )

install(TARGETS ktlswitches DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES ktlswitches.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
#install( FILES ktlswitches.rc DESTINATION ${DATA_INSTALL_DIR}/ktlswitches )


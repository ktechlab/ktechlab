add_subdirectory(themes)

########### next target ###############
include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/src/lib
)

set( ktllogic_PART_SRCS
   ktllogiccomponentsplugin.cpp
)

kde4_add_plugin(ktllogic_components ${ktllogic_PART_SRCS})
target_link_libraries(ktllogic_components
   ${KDE4_KDECORE_LIBS}
   ktlinterfaces
   ktlcircuit
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   )

install(TARGETS ktllogic_components DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES ktllogic_components.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES ktllogic_components.rc DESTINATION ${DATA_INSTALL_DIR}/ktechlab/components/ )

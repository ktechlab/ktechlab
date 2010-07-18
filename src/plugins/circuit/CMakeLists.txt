add_subdirectory( themes )

########### next target ###############
include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/src/lib/circuit
)

set( ktlcircuit_PART_SRCS
   componenteditorview.cpp
   circuitscene.cpp
   ktlcircuitplugin.cpp
   circuitdocument.cpp
   circuitview.cpp
)

kde4_add_plugin(ktlcircuitplugin ${ktlcircuit_PART_SRCS})
target_link_libraries(ktlcircuitplugin
   ${KDE4_KDECORE_LIBS}
   ktlinterfaces
   ktlcircuit
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   ${KDEVPLATFORM_SUBLIME_LIBRARIES}
   ${KDE4_KTEXTEDITOR_LIBS})

install(TARGETS ktlcircuitplugin DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

ktl_install_desktop( ktlcircuit.desktop )
#install( FILES ktlcircuit.rc DESTINATION ${DATA_INSTALL_DIR}/ktlcircuit )


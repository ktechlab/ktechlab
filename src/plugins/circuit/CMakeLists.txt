add_subdirectory( themes )

########### next target ###############
include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/src/lib/circuit
)

set( ktlcircuit_qt_SRCS
   fakecomponentitemfactory.cpp
   circuitdocument.cpp
   circuitscene.cpp
   circuitview.cpp
   # TODO create and add ktlcircuitplugin_qt.cpp
)

set( ktlcircuit_PART_SRCS
   ktlcircuitplugin.cpp
   componenteditorview.cpp
   ${ktlcircuit_qt_SRCS}
)

add_library(ktlcircuitplugin_qt STATIC ${ktlcircuit_qt_SRCS})
set_target_properties(ktlcircuitplugin_qt  PROPERTIES COMPILE_FLAGS "-DKDE_ENABLED=0")

kde4_add_plugin(ktlcircuitplugin ${ktlcircuit_PART_SRCS})
set_target_properties(ktlcircuitplugin  PROPERTIES COMPILE_FLAGS "-DKDE_ENABLED=1")
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
install( FILES ktlcircuitui.rc DESTINATION ${DATA_INSTALL_DIR}/ktlcircuit )


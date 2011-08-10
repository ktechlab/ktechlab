# building of the simulator plugin

include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/interfaces
)

set( ktlsimulator_PART_SRCS
   pingroup.cpp
   simulatorplugin.cpp
   simulatorfactory.cpp
   elementset.cpp
   circuittransientsimulator.cpp
)

kde4_add_plugin( ktlsimulatorplugin ${ktlsimulator_PART_SRCS} )

target_link_libraries( ktlsimulatorplugin
    ktlinterfaces
    ktlmath
    ${KDE4_KDECORE_LIBS}
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    )

install(TARGETS ktlsimulatorplugin DESTINATION ${PLUGIN_INSTALL_DIR} )

ktl_install_desktop( ktlsimulatorplugin.desktop )


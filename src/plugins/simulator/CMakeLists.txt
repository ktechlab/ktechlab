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

kde4_add_plugin( ktlsimulator ${ktlsimulator_PART_SRCS} )

target_link_libraries( ktlsimulator
    ktlinterfaces
    ktlmath
    ${KDE4_KDECORE_LIBS}
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    )

install(TARGETS ktlsimulator DESTINATION ${PLUGIN_INSTALL_DIR} )

ktl_install_desktop( ktlsimulator.desktop )


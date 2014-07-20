# building of the simulator plugin

include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/src/lib/simulator
   ${CMAKE_SOURCE_DIR}/src/lib/simulator/electronics
   ${CMAKE_SOURCE_DIR}/src/lib/simulator/electronics/components
   ${CMAKE_SOURCE_DIR}/src/lib/simulator/electronics/simulation
   ${CMAKE_SOURCE_DIR}/src/lib/simulator/math
   )

set( ktlsimulator_PART_SRCS
   simulatorplugin.cpp
   simulatorfactory.cpp
   circuittransientsimulator.cpp
)

if(KDEVPLATFORM_ENABLED)

kde4_add_plugin( ktlsimulatorplugin_kde ${ktlsimulator_PART_SRCS} )
set_target_properties(ktlsimulatorplugin_kde PROPERTIES COMPILE_FLAGS "-DKDE_ENABLED=1")

target_link_libraries( ktlsimulatorplugin_kde
    ktlinterfaces
    ktlmath
    ${KDE4_KDECORE_LIBS}
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
    )

install(TARGETS ktlsimulatorplugin_kde DESTINATION ${PLUGIN_INSTALL_DIR} )

ktl_install_desktop( ktlsimulatorplugin.desktop )

endif(KDEVPLATFORM_ENABLED)

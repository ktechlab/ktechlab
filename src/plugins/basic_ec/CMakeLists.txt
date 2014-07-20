add_subdirectory( themes )

########### next target ###############
include_directories(
   ${CMAKE_SOURCE_DIR}/src
   ${CMAKE_SOURCE_DIR}/src/lib
)

set( ktlbasic_ec_PART_SRCS
   ktlbasicecplugin.cpp
   elements/resistance.cpp
   elements/capacitance.cpp
   elements/voltagesource.cpp
)

if(KDEVPLATFORM_ENABLED)

kde4_add_plugin(ktlbasic_ec ${ktlbasic_ec_PART_SRCS})
set_target_properties(ktlbasic_ec PROPERTIES COMPILE_FLAGS "-DKDE_ENABLED=1")
target_link_libraries(ktlbasic_ec
   ${KDE4_KDECORE_LIBS}
   ${KDE4_PLASMA_LIBS}
   ktlinterfaces
   ktlcircuit
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   )

install(TARGETS ktlbasic_ec DESTINATION ${PLUGIN_INSTALL_DIR} )



########### install files ###############

ktl_install_desktop( ktlbasic_ec.desktop )

endif(KDEVPLATFORM_ENABLED)

install( FILES ktlbasic_ec.rc DESTINATION ${DATA_INSTALL_DIR}/ktechlab/components/ )


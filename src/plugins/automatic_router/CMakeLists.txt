include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

set( ktlautomatic_router_PART_SRCS
    cells.cpp
    ktlautomaticrouterplugin.cpp
)

kde4_add_plugin(ktlautomatic_router ${ktlautomatic_router_PART_SRCS})
target_link_libraries(ktlautomatic_router
    ktlinterfaces
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
)

install(TARGETS ktlautomatic_router DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES ktlautomatic_router.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

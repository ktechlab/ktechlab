include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

set( simple_router_SRCS
    simpleroutinginformation.cpp
    simplerouterplugin.cpp
)

kde4_add_plugin(ktlsimple_router ${simple_router_SRCS})
target_link_libraries(ktlsimple_router
    ktlinterfaces
    ${KDEVPLATFORM_INTERFACES_LIBRARIES}
)

install(TARGETS ktlsimple_router DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES ktlsimple_router.desktop DESTINATION ${SERVICES_INSTALL_DIR} )

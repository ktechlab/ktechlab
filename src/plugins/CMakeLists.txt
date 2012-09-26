add_subdirectory( circuit )
add_subdirectory( basic_ec )
add_subdirectory( simulator )

install( FILES ktechlabplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )

add_subdirectory(automatic_router)
add_subdirectory(logic_components)
add_subdirectory(simple_router)
add_subdirectory(ktlgui)
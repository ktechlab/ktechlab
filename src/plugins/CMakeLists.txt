add_subdirectory( circuit )
add_subdirectory( basic_ec )

install( FILES ktechlabplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )

add_subdirectory(automatic_router)
add_subdirectory( circuit )
add_subdirectory( basic_ec )
add_subdirectory( switches )
add_subdirectory( passive )

install( FILES ktechlabplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR} )

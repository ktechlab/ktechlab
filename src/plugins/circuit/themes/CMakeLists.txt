FILE(GLOB ktechlab_svgs *.svg)
install( FILES ${ktechlab_svgs} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/themes )

FILE(GLOB ktechlab_configs *.config)
install( FILES ${ktechlab_configs} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/themes )

FILE(GLOB ktechlab_svgs components/*.svg)
install( FILES ${ktechlab_svgs} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/themes/components )

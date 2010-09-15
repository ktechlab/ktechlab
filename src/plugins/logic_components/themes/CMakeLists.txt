FILE(GLOB ktechlab_svgs *.svgz)
install( FILES ${ktechlab_svgs} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/themes )

FILE(GLOB ktechlab_configs *.config)
install( FILES ${ktechlab_configs} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/themes )

FILE(GLOB ktechlab_svgs components/din/*.svg)
install( FILES ${ktechlab_svgs} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/themes/din/components )

FILE(GLOB ktechlab_svgs components/us/*.svg)
install( FILES ${ktechlab_svgs} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/themes/us/components )

FILE(GLOB ktechlab_pics pics/*)
install( FILES ${ktechlab_pics} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/pics )

FILE(GLOB ktechlab_svgs *.svgz)
install( FILES ${ktechlab_svgs} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/ktechlab/ )

FILE(GLOB ktechlab_configs *.config)
install( FILES ${ktechlab_configs} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/ktechlab/ )

FILE(GLOB ktechlab_svgs components/din/*.svgz)
install( FILES ${ktechlab_svgs} DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/ktechlab/components/din )

FILE(GLOB ktechlab_pics pics/*)
install( FILES ${ktechlab_pics} DESTINATION ${DATA_INSTALL_DIR}/ktechlab/pics )

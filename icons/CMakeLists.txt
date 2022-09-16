add_subdirectory(pics)

########### install files ###############

file(GLOB icons_apps "${CMAKE_CURRENT_SOURCE_DIR}/*-apps-*.png")
file(GLOB icons_mimetypes "${CMAKE_CURRENT_SOURCE_DIR}/*-mimetypes-*.png")
ecm_install_icons(ICONS ${icons_apps} ${icons_mimetypes} DESTINATION ${KDE_INSTALL_ICONDIR})

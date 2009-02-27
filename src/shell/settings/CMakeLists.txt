
include_directories(
    ${CMAKE_SOURCE_DIR}
)

########### Plugin Selector
set( pluginsettings_cfg_SRCS
     pluginpreferences.cpp
)

kde4_add_plugin( kcm_ktl_pluginsettings ${pluginsettings_cfg_SRCS} )
target_link_libraries( kcm_ktl_pluginsettings ${KDE4_KUTILS_LIBS} ${KDE4_KDEUI_LIBS} ktlshell )

########### Project Configuration

set( projectsettings_cfg_SRCS
    projectpreferences.cpp
)

set( projectsettings_cfg_UI
    projectsettings.ui
)

kde4_add_ui_files( projectsettings_cfg_SRCS ${projectsettings_cfg_UI} )
kde4_add_kcfg_files( projectsettings_cfg_SRCS projectconfig.kcfgc )
kde4_add_plugin( kcm_ktl_projectsettings ${projectsettings_cfg_SRCS} )
target_link_libraries( kcm_ktl_projectsettings ${KDE4_KUTILS_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ktlshell)

install( TARGETS
        kcm_ktl_projectsettings
        kcm_ktl_pluginsettings
        DESTINATION ${PLUGIN_INSTALL_DIR} )

install( FILES
        kcm_ktl_pluginsettings.desktop
        kcm_ktl_projectsettings.desktop
        DESTINATION ${SERVICES_INSTALL_DIR} )

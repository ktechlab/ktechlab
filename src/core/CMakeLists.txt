SET(core_STAT_SRCS
#    diagnosticstyle.cpp
#    main.cpp
)

set(core_KCFG_SRCS
    ktlconfig.kcfgc
)

kconfig_add_kcfg_files(core_STAT_SRCS ${core_KCFG_SRCS})

add_library(core STATIC ${core_STAT_SRCS})
target_link_libraries(core
    KF6::ConfigGui
    Qt6::Core5Compat
)

########### install files ###############

install(FILES ktechlab.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR})

SET(core_STAT_SRCS
   logtofilemsghandler.cpp
   diagnosticstyle.cpp
   main.cpp
)

set(core_KCFG_SRCS
    ktlconfig.kcfgc
)

kde4_add_kcfg_files(core_STAT_SRCS ${core_KCFG_SRCS})

kde4_add_library(core STATIC ${core_STAT_SRCS})


########### install files ###############

install(FILES ktechlab.kcfg DESTINATION ${KCFG_INSTALL_DIR})

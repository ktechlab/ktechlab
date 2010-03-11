########### next target ###############

set(core_KCFG_SRCS
    ktlconfig.kcfgc
)

kde4_add_kcfg_files(core_STAT_SRCS ${core_KCFG_SRCS})


########### install files ###############

install(FILES  ktechlab.kcfg DESTINATION share/config.kcfg)

install( FILES  profile.config  DESTINATION ${DATA_INSTALL_DIR}/kdevplatform/profiles/ktechlab )

#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/drawparts 	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components 	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui 	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(all_includes)
#METASOURCES = AUTO
#libcore_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES = libcore.la
#kde_kcfg_DATA = ktechlab.kcfg
#libcore_la_SOURCES = ktlconfig.kcfgc main.cpp

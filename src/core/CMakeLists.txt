include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/drawparts ${CMAKE_SOURCE_DIR}/src/electronics ${CMAKE_SOURCE_DIR}/src/electronics/components ${CMAKE_SOURCE_DIR}/src/electronics/simulation ${CMAKE_SOURCE_DIR}/src/flowparts ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/mechanics ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(core_STAT_SRCS
   logtofilemsghandler.cpp
   diagnosticstyle.cpp
   main.cpp
)

qt4_automoc(${core_STAT_SRCS})

set(core_KCFG_SRCS
    ktlconfig.kcfgc
)

kde4_add_kcfg_files(core_STAT_SRCS ${core_KCFG_SRCS})

add_library(core STATIC ${core_STAT_SRCS})


########### install files ###############

install(FILES  ktechlab.kcfg DESTINATION share/config.kcfg)



#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/drawparts 	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components 	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui 	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(all_includes)
#METASOURCES = AUTO
#libcore_la_LDFLAGS = $(all_libraries)
#noinst_LTLIBRARIES = libcore.la
#kde_kcfg_DATA = ktechlab.kcfg
#libcore_la_SOURCES = ktlconfig.kcfgc main.cpp

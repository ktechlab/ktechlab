include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/electronics ${CMAKE_SOURCE_DIR}/src/electronics/components ${CMAKE_SOURCE_DIR}/src/electronics/simulation ${CMAKE_SOURCE_DIR}/src/flowparts ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/mechanics ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(drawparts_STAT_SRCS
   drawpart.cpp
   dpline.cpp
   solidshape.cpp
   dptext.cpp
   dpimage.cpp
)

qt4_automoc(${drawparts_STAT_SRCS})

add_library(drawparts STATIC ${drawparts_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src 	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components 	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui 	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(all_includes)
#METASOURCES = AUTO
#noinst_LTLIBRARIES = libdrawparts.la
#libdrawparts_la_SOURCES = drawpart.cpp dpline.cpp solidshape.cpp dptext.cpp \
#	dpimage.cpp
#noinst_HEADERS = drawpart.h dpline.h solidshape.h dptext.h dpimage.h
#

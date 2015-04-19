include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/electronics ${CMAKE_SOURCE_DIR}/src/electronics/components ${CMAKE_SOURCE_DIR}/src/electronics/simulation ${CMAKE_SOURCE_DIR}/src/flowparts ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/mechanics ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(math_STAT_SRCS
   qvector.cpp
   qmatrix.cpp
)

qt4_automoc(${math_STAT_SRCS})

add_library(math STATIC ${math_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/electronics \
#	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/simulation \
#	-I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/languages \
#	-I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(glib_cflags) $(all_includes)
#
#METASOURCES = AUTO
#
#SUBDIRS = 
#
#noinst_LTLIBRARIES = libmath.la
#
#libmath_la_SOURCES = qvector.cpp qmatrix.cpp
#
#libmath_la_LIBADD = 
#
#
#noinst_HEADERS = qvector.h qmatrix.h

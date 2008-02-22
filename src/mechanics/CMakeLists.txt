include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/drawparts ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(mechanics_STAT_SRCS
   mechanicsitem.cpp
   chassiscircular2.cpp
   mechanicssimulation.cpp
   mechanicsdocument.cpp
   mechanicsgroup.cpp
   mechanicsview.cpp
)

kde3_automoc(${mechanics_STAT_SRCS})

add_library(mechanics STATIC ${mechanics_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/drawparts $(all_includes)
#METASOURCES = AUTO
#noinst_HEADERS = mechanicsitem.h chassiscircular2.h mechanicssimulation.h \
#	mechanicsdocument.h mechanicsgroup.h mechanicsview.h
#
#noinst_LTLIBRARIES =  libmechanics.la
#libmechanics_la_SOURCES = mechanicsitem.cpp chassiscircular2.cpp \
#		mechanicssimulation.cpp mechanicsdocument.cpp mechanicsgroup.cpp mechanicsview.cpp

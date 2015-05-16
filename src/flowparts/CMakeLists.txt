include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/electronics ${CMAKE_SOURCE_DIR}/src/electronics/components ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(flowparts_STAT_SRCS
   callsub.cpp
   delay.cpp
   end.cpp
   forloop.cpp
   readport.cpp
   setpin.cpp
   start.cpp
   testpin.cpp
   unary.cpp
   varassignment.cpp
   varcomparison.cpp
   writeport.cpp
   repeat.cpp
   while.cpp
   sub.cpp
   count.cpp
   embed.cpp
   interrupt.cpp
   keypad.cpp
   pulse.cpp
   sevenseg.cpp
   inputbutton.cpp
   flowpart.cpp
   pinmapping.cpp
   flowconnector.cpp
   flowconnectorlist.cpp
   flowicndocument.cpp
   inputflownode.cpp
   outputflownode.cpp
   junctionflownode.cpp
)

qt4_automoc(${flowparts_STAT_SRCS})

add_library(flowparts STATIC ${flowparts_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/electronics \
#	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/languages \
#	-I$(top_srcdir)/src/micro $(all_includes)
#METASOURCES = AUTO
#noinst_LTLIBRARIES =  libflowparts.la
#noinst_HEADERS = callsub.h delay.h end.h forloop.h readport.h setpin.h start.h \
#			testpin.h unary.h varassignment.h varcomparison.h writeport.h repeat.h while.h \
#			sub.h inputbutton.h flowpart.h pinmapping.h
#libflowparts_la_SOURCES = callsub.cpp delay.cpp end.cpp forloop.cpp \
#		readport.cpp 	setpin.cpp start.cpp testpin.cpp unary.cpp varassignment.cpp \
#		varcomparison.cpp 	writeport.cpp repeat.cpp while.cpp sub.cpp count.cpp embed.cpp \
#		interrupt.cpp 	keypad.cpp pulse.cpp sevenseg.cpp inputbutton.cpp flowpart.cpp \
#	pinmapping.cpp
#
#libflowparts_la_PCH = AUTO

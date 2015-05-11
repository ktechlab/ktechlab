add_subdirectory(simulation)
add_subdirectory(components)
add_subdirectory(models)

include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/drawparts
        ${CMAKE_SOURCE_DIR}/src/electronics
        ${CMAKE_SOURCE_DIR}/src/electronics/components
        ${CMAKE_SOURCE_DIR}/src/electronics/simulation
        ${CMAKE_SOURCE_DIR}/src/flowparts
        ${CMAKE_SOURCE_DIR}/src/gui
        ${CMAKE_SOURCE_DIR}/src/languages
        ${CMAKE_SOURCE_DIR}/src/mechanics
        ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KDE3_INCLUDE_DIR}
        ${QT_INCLUDE_DIR} )


########### next target ###############

SET(electronics_STAT_SRCS
   component.cpp
   subcircuits.cpp
   gpsimprocessor.cpp
   switch.cpp
   pin.cpp
   wire.cpp
   ecnode.cpp
   port.cpp
   componentmodellibrary.cpp
   circuitdocument.cpp
   pinnode.cpp
   circuiticndocument.cpp
   junctionnode.cpp
   electronicconnector.cpp
)

qt4_automoc(${electronics_STAT_SRCS})

include( CheckCXXCompilerFlag )
CHECK_CXX_COMPILER_FLAG( -fno-exceptions CXXFLAG_NOEXCEPTIONS )

if( CXXFLAG_NOEXCEPTIONS )
	ADD_DEFINITIONS( -fexceptions )
endif( CXXFLAG_NOEXCEPTIONS )

add_library(electronics STATIC ${electronics_STAT_SRCS})
target_link_libraries( electronics elements components )

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/electronics \
#	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/simulation \
#	-I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/languages \
#	-I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(glib_cflags) $(all_includes)
#
#METASOURCES = AUTO
#
#SUBDIRS = simulation components models
#
#noinst_LTLIBRARIES = libelectronics.la
#
#libelectronics_la_SOURCES = component.cpp subcircuits.cpp gpsimprocessor.cpp \
#		switch.cpp pin.cpp wire.cpp ecnode.cpp port.cpp componentmodellibrary.cpp
#
#libelectronics_la_LIBADD = \
#	$(top_builddir)/src/electronics/simulation/libelements.la $(top_builddir)/src/electronics/components/libcomponents.la
#
#noinst_HEADERS = gpsimprocessor.h switch.h pin.h wire.h ecnode.h port.h \
#	componentmodellibrary.h

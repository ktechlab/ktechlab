INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/electronics \
	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/simulation \
	-I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/languages \
	-I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(glib_cflags) $(all_includes)

METASOURCES = AUTO

SUBDIRS = simulation components models

noinst_LTLIBRARIES = libelectronics.la

libelectronics_la_SOURCES = circuiticndocument.cpp component.cpp \
	componentmodellibrary.cpp ecnode.cpp gpsimprocessor.cpp junctionnode.cpp pin.cpp pinnode.cpp \
	port.cpp subcircuits.cpp switch.cpp wire.cpp

libelectronics_la_LIBADD = \
	$(top_builddir)/src/electronics/simulation/libelements.la $(top_builddir)/src/electronics/components/libcomponents.la

noinst_HEADERS = circuiticndocument.h componentmodellibrary.h ecnode.h \
	gpsimprocessor.h junctionnode.h pin.h pinnode.h port.h switch.h wire.h

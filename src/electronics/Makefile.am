INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/electronics \
	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/simulation \
	-I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/languages \
	-I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(glib_cflags) $(all_includes)

METASOURCES = AUTO

SUBDIRS = simulation components

noinst_LTLIBRARIES = libelectronics.la

libelectronics_la_SOURCES = component.cpp subcircuits.cpp gpsimprocessor.cpp \
	switch.cpp pin.cpp wire.cpp ecnode.cpp port.cpp

libelectronics_la_LIBADD = \
	$(top_builddir)/src/electronics/simulation/libelements.la $(top_builddir)/src/electronics/components/libcomponents.la

noinst_HEADERS = gpsimprocessor.h switch.h pin.h wire.h ecnode.h port.h

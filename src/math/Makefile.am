INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/drawparts 	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components 	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui 	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES = libtechmath.la
libtechmath_la_LDFLAGS = -no-undefined $(all_libraries)
libtechmath_la_SOURCES = qmatrix.cpp qvector.cpp


noinst_HEADERS = qmatrix.h qvector.h

INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/electronics \
	-I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/electronics/simulation \
	-I$(top_srcdir)/src/flowparts -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/languages \
	-I$(top_srcdir)/src/mechanics -I$(top_srcdir)/src/micro $(glib_cflags) $(all_includes)

METASOURCES = AUTO

SUBDIRS = 

noinst_LTLIBRARIES = libmath.la

libmath_la_SOURCES = qvector.cpp qmatrix.cpp

libmath_la_LIBADD = 


noinst_HEADERS = qvector.h qmatrix.h

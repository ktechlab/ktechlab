INCLUDES = -I$(top_srcdir)/src/math
METASOURCES = AUTO
noinst_PROGRAMS = mathtest
mathtest_SOURCES = mathtest.cpp
mathtest_LDADD = $(top_builddir)/src/math/libtechmath.la \
	-lkutils
	
mathtest_LDFLAGS = $(KDE_RPATH) $(all_libraries)

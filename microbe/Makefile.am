INCLUDES = $(all_includes)
METASOURCES = AUTO
bin_PROGRAMS = microbe
microbe_LDFLAGS = $(all_libraries) $(KDE_RPATH)
microbe_SOURCES = btreebase.cpp btreenode.cpp main.cpp traverser.cpp \
	expression.cpp pic14.cpp variable.cpp optimizer.cpp instruction.cpp microbe.cpp \
	parser.cpp
noinst_HEADERS = btreebase.h btreenode.h traverser.h pic14.h variable.h \
	optimizer.h microbe.h parser.h
microbe_LDADD = -lpthread $(LIB_KFILE)

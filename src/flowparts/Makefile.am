INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/drawparts \
	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components -I$(top_srcdir)/src/gui \
	-I$(top_srcdir)/src/languages -I$(top_srcdir)/src/micro $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES =  libflowparts.la
noinst_HEADERS = callsub.h delay.h end.h flowcodedocument.cpp flowconnector.h \
	flowconnectorlist.h flowicndocument.h flowpart.h forloop.h fpnode.h inputbutton.h \
	inputflownode.h junctionflownode.h outputflownode.h pinmapping.h readport.h repeat.h \
	setpin.h start.h sub.h testpin.h unary.h varassignment.h varcomparison.h while.h \
	writeport.h
libflowparts_la_SOURCES = 	keypad.cpp 	setpin.cpp 	writeport.cpp callsub.cpp \
	count.cpp delay.cpp embed.cpp end.cpp flowcodedocument.cpp flowconnector.cpp \
	flowicndocument.cpp flowpart.cpp forloop.cpp fpnode.cpp inputbutton.cpp inputflownode.cpp \
	interrupt.cpp junctionflownode.cpp keypad.cpp outputflownode.cpp pinmapping.cpp \
	pulse.cpp readport.cpp repeat.cpp sevenseg.cpp start.cpp sub.cpp testpin.cpp \
	unary.cpp varassignment.cpp varcomparison.cpp while.cpp

libflowparts_la_PCH = AUTO

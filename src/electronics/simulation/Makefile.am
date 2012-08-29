INCLUDES = -I$(top_srcdir)/src -I$(top_srcdir)/src/electronics \
           -I$(top_srcdir)/src/core $(all_includes)
METASOURCES = AUTO
noinst_LTLIBRARIES =  libelements.la
libelements_la_SOURCES = cccs.cpp ccvs.cpp circuit.cpp currentsource.cpp \
	diode.cpp element.cpp elementset.cpp logic.cpp matrix.cpp vccs.cpp vcvs.cpp \
	voltagesource.cpp capacitance.cpp resistance.cpp currentsignal.cpp voltagepoint.cpp \
	voltagesignal.cpp elementsignal.cpp nonlinear.cpp reactive.cpp bjt.cpp opamp.cpp \
	inductance.cpp jfet.cpp mosfet.cpp
noinst_HEADERS = cccs.h ccvs.h circuit.h currentsource.h diode.h element.h \
	elementset.h logic.h matrix.h vccs.h vcvs.h voltagesource.h capacitance.h \
	resistance.h elementsignal.h nonlinear.h reactive.h bjt.h opamp.h inductance.h \
	jfet.h mosfet.h

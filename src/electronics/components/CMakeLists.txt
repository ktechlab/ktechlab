include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/electronics ${CMAKE_SOURCE_DIR}/src/electronics/components ${CMAKE_SOURCE_DIR}/src/electronics/simulation ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(components_STAT_SRCS
   ecdiode.cpp
   ecsevensegment.cpp
   eckeypad.cpp
   ec555.cpp
   eccurrentsource.cpp
   ecfixedvoltage.cpp
   ecbcdto7segment.cpp
   ecsignallamp.cpp
   ecclockinput.cpp
   ecpotentiometer.cpp
   ecopamp.cpp
   ecvoltagesource.cpp
   eccurrentsignal.cpp
   ecvoltagesignal.cpp
   ecground.cpp
   multiinputgate.cpp
   fulladder.cpp
   multiplexer.cpp
   demultiplexer.cpp
   externalconnection.cpp
   ecsubcircuit.cpp
   meter.cpp
   probe.cpp
   resistordip.cpp
   addac.cpp
   dependentsource.cpp
   flipflop.cpp
   toggleswitch.cpp
   pushswitch.cpp
   ram.cpp
   discretelogic.cpp
   piccomponent.cpp
   piccomponentpin.cpp
   binarycounter.cpp
   bidirled.cpp
   matrixdisplay.cpp
   bussplitter.cpp
   matrixdisplaydriver.cpp
   magnitudecomparator.cpp
   serialportcomponent.cpp
   parallelportcomponent.cpp
   inductor.cpp
   ecbjt.cpp
   rotoswitch.cpp
   ecjfet.cpp
   ecmosfet.cpp
   variablecapacitor.cpp
   variableresistor.cpp
   capacitor.cpp
   resistor.cpp
   voltageregulator.cpp
   ledbargraphdisplay.cpp
   led.cpp
)

kde3_automoc(${components_STAT_SRCS})

include( CheckCXXCompilerFlag )
CHECK_CXX_COMPILER_FLAG( -fno-exceptions CXXFLAG_NOEXCEPTIONS )

if( CXXFLAG_NOEXCEPTIONS )
	ADD_DEFINITIONS( -fexceptions )
endif( CXXFLAG_NOEXCEPTIONS )

add_library(components STATIC ${components_STAT_SRCS})
target_link_libraries( components elements )

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src \
#	-I$(top_srcdir)/src/electronics -I$(top_srcdir)/src/electronics/components \
#	-I$(top_srcdir)/src/electronics/simulation -I$(top_srcdir)/src/gui -I$(top_srcdir)/src/languages \
#	-I$(top_srcdir)/src/micro $(glib_cflags) $(all_includes)
#
#METASOURCES = AUTO
#noinst_HEADERS = ecdiode.h ecsevensegment.h eckeypad.h ec555.h \
#	eccurrentsource.h ecfixedvoltage.h ecbcdto7segment.h ecsignallamp.h ecclockinput.h \
#	ecpotentiometer.h ecopamp.h ecvoltagesource.h eccurrentsignal.h ecvoltagesignal.h \
#	ecground.h multiinputgate.h fulladder.h multiplexer.h demultiplexer.h \
#	externalconnection.h ecsubcircuit.h meter.h probe.h resistordip.h addac.h dependentsource.h \
#	flipflop.h toggleswitch.h pushswitch.h ram.h discretelogic.h piccomponent.h \
#	piccomponentpin.h binarycounter.h bidirled.h matrixdisplay.h bussplitter.h \
#	matrixdisplaydriver.h magnitudecomparator.h serialportcomponent.h parallelportcomponent.h \
#	inductor.h ecbjt.h rotoswitch.h ecjfet.h ecmosfet.h variablecapacitor.h \
#	variableresistor.h voltageregulator.h ledbargraphdisplay.h led.h
#
#noinst_LTLIBRARIES =  libcomponents.la
#libcomponents_la_SOURCES = ecdiode.cpp ecsevensegment.cpp eckeypad.cpp \
#	ec555.cpp eccurrentsource.cpp ecfixedvoltage.cpp ecbcdto7segment.cpp \
#	ecsignallamp.cpp ecclockinput.cpp ecpotentiometer.cpp ecopamp.cpp ecvoltagesource.cpp \
#	eccurrentsignal.cpp ecvoltagesignal.cpp ecground.cpp multiinputgate.cpp fulladder.cpp \
#	multiplexer.cpp demultiplexer.cpp externalconnection.cpp ecsubcircuit.cpp meter.cpp \
#	probe.cpp resistordip.cpp addac.cpp dependentsource.cpp flipflop.cpp \
#	toggleswitch.cpp pushswitch.cpp ram.cpp discretelogic.cpp piccomponent.cpp \
#	piccomponentpin.cpp binarycounter.cpp bidirled.cpp matrixdisplay.cpp bussplitter.cpp \
#	matrixdisplaydriver.cpp magnitudecomparator.cpp serialportcomponent.cpp \
#	parallelportcomponent.cpp inductor.cpp ecbjt.cpp rotoswitch.cpp ecjfet.cpp ecmosfet.cpp \
#	variablecapacitor.cpp variableresistor.cpp capacitor.cpp resistor.cpp voltageregulator.cpp \
#	ledbargraphdisplay.cpp led.cpp
#
#libcomponents_la_PCH = AUTO
#
#
#libcomponents_la_LIBADD =\
#	$(top_builddir)/src/electronics/simulation/libelements.la

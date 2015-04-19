include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(micro_STAT_SRCS
   microinfo.cpp
   picinfo.cpp
   picinfo16bit.cpp
   picinfo14bit.cpp
   picinfo12bit.cpp
   microlibrary.cpp
   micropackage.cpp
   asminfo.cpp
)

qt4_automoc(${micro_STAT_SRCS})

add_library(micro STATIC ${micro_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#noinst_LTLIBRARIES = libmicro.la
#noinst_HEADERS = microinfo.h picinfo.h picinfo16bit.h picinfo14bit.h \
#	picinfo12bit.h microlibrary.h asminfo.h
#libmicro_la_SOURCES = microinfo.cpp picinfo.cpp picinfo16bit.cpp \
#	picinfo14bit.cpp picinfo12bit.cpp microlibrary.cpp micropackage.cpp asminfo.cpp

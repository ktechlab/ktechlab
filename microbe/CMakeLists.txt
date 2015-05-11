include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(microbe_SRCS
   btreebase.cpp
   btreenode.cpp
   main.cpp
   traverser.cpp
   expression.cpp
   pic14.cpp
   variable.cpp
   optimizer.cpp
   instruction.cpp
   microbe.cpp
   parser.cpp
)

qt4_automoc(${microbe_SRCS})

add_executable(microbe ${microbe_SRCS})

target_link_libraries(microbe ${QT_AND_KDECORE_LIBS} ${QT_QTCORE_LIBRARY} ${KDE4_KDECORE_LIBRARY} pthread ${LIB_KFILE})

install(TARGETS microbe DESTINATION bin)


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#bin_PROGRAMS = microbe
#microbe_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#microbe_SOURCES = btreebase.cpp btreenode.cpp main.cpp traverser.cpp \
#	expression.cpp pic14.cpp variable.cpp optimizer.cpp instruction.cpp microbe.cpp \
#	parser.cpp
#noinst_HEADERS = btreebase.h btreenode.h traverser.h pic14.h variable.h \
#	optimizer.h microbe.h parser.h
#microbe_LDADD = -lpthread $(LIB_KFILE)

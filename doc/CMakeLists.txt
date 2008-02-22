message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(AUTODIRS)")
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############




#original Makefile.am contents follow:

## the SUBDIRS is filled automatically by am_edit. If files are 
## in this directory they are installed into the english dir
#
#KDE_LANG = en
#KDE_DOCS = AUTO
#SUBDIRS = $(AUTODIRS)
#

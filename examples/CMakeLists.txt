add_subdirectory(555)
add_subdirectory(basic)
add_subdirectory(diodes)
add_subdirectory(jfets)
add_subdirectory(mosfets)
add_subdirectory(transistors)
add_subdirectory(opamps)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = 
#METASOURCES = AUTO
#SUBDIRS = 555 basic diodes jfets mosfets transistors opamps

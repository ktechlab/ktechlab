include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(languages_STAT_SRCS
   language.cpp
   languagemanager.cpp
   microbe.cpp
   externallanguage.cpp
   gpasm.cpp
   gpdasm.cpp
   processchain.cpp
   flowcode.cpp
   asmparser.cpp
   sdcc.cpp
   gplink.cpp
   gplib.cpp
   sourceline.cpp
   picprogrammer.cpp
)

kde4_add_library(languages STATIC ${languages_STAT_SRCS})
target_link_libraries( languages gui )

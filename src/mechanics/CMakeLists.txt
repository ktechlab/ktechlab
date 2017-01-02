SET(mechanics_STAT_SRCS
   mechanicsitem.cpp
   chassiscircular2.cpp
   mechanicssimulation.cpp
   mechanicsdocument.cpp
   mechanicsgroup.cpp
   mechanicsview.cpp
)

kde4_add_library(mechanics STATIC ${mechanics_STAT_SRCS})

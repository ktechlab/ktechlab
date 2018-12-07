SET(ktlqt3support_STAT_SRCS
   ktlq3scrollview.cpp
   ktlq3polygonscanner.cpp
   ktlq3frame.cpp
   ktlfindqobjectchild.cpp
)

kde4_add_library(ktlqt3support STATIC ${ktlqt3support_STAT_SRCS})

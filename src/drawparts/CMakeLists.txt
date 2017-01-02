SET(drawparts_STAT_SRCS
   drawpart.cpp
   dpline.cpp
   solidshape.cpp
   dptext.cpp
   dpimage.cpp
)

kde4_add_library(drawparts STATIC ${drawparts_STAT_SRCS})

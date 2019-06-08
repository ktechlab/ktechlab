SET(drawparts_STAT_SRCS
#    drawpart.cpp
#    dpline.cpp
#    solidshape.cpp
#    dptext.cpp
#    dpimage.cpp
)

add_library(drawparts STATIC ${drawparts_STAT_SRCS})

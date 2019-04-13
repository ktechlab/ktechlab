SET(math_STAT_SRCS
   qvector.cpp
   qmatrix.cpp
)

add_library(math STATIC ${math_STAT_SRCS})

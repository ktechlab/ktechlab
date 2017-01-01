SET(math_STAT_SRCS
   qvector.cpp
   qmatrix.cpp
)

kde4_add_library(math STATIC ${math_STAT_SRCS})

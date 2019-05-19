SET(micro_STAT_SRCS
#    microinfo.cpp
#    picinfo.cpp
#    picinfo16bit.cpp
#    picinfo14bit.cpp
#    picinfo12bit.cpp
#    microlibrary.cpp
#    micropackage.cpp
#    asminfo.cpp
)

add_library(micro STATIC ${micro_STAT_SRCS})

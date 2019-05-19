add_subdirectory(simulation)
add_subdirectory(components)
add_subdirectory(models)

########### next target ###############

SET(electronics_STAT_SRCS
#    component.cpp
#    subcircuits.cpp
#    gpsimprocessor.cpp
#    switch.cpp
#    pin.cpp
#    wire.cpp
#    ecnode.cpp
#    port.cpp
#    componentmodellibrary.cpp
#    circuitdocument.cpp
#    pinnode.cpp
#    circuiticndocument.cpp
#    junctionnode.cpp
#    electronicconnector.cpp
)

add_library(electronics STATIC ${electronics_STAT_SRCS})
target_link_libraries( electronics elements components )

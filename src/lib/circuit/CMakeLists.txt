include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

set(circuit_SRCS
    componentitem.cpp
    pinitem.cpp
    theme.cpp
    circuitmodel.cpp
    simulator/genericelementfactory.cpp
)

kde4_add_library( circuit STATIC ${circuit_SRCS} )

target_link_libraries(circuit
    ktlinterfaces
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTSVG_LIBRARY}
    ${KDE4_KDECORE_LIBS}
)

install()

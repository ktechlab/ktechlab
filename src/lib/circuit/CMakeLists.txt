include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

set(circuit_SRCS
    genericcomponentitemfactory.cpp
    componentitem.cpp
    pinitem.cpp
    theme.cpp
    circuitmodel.cpp
    simulator/genericelementfactory.cpp
)

add_library(ktlcircuit_qt STATIC ${circuit_SRCS} )

kde4_add_library( ktlcircuit SHARED ${circuit_SRCS} )

target_link_libraries(ktlcircuit
    ktlinterfaces
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QTSVG_LIBRARY}
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
)

install(TARGETS ktlcircuit ${INSTALL_TARGETS_DEFAULT_ARGS})

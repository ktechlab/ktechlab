
# for qt only executable


set( ktechlab_qt_SRCS
	mainqt.cpp
	mainwindowqt.cpp
	)

set ( ktechlab_qt_HEADERS_TO_MOC
	mainwindowqt.h
	)

qt4_wrap_cpp(ktechlab_qt_MOCED_HEADERS ${ktechlab_qt_HEADERS_TO_MOC})

SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTSVG TRUE)

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

include_directories(
    ${QT_INCLUDES}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/interfaces
	)

add_executable(ktechlab_qt
	${ktechlab_qt_SRCS}
	${ktechlab_qt_MOCED_HEADERS}
	)
set_target_properties(ktechlab_qt PROPERTIES COMPILE_FLAGS "-DKDE_ENABLED=0")

target_link_libraries(ktechlab_qt
	ktlcircuitplugin_qt
	ktlcircuit_qt
	ktlinterfaces_qt
	ktlsimulator_static
	ktlmath_static
	${QT_LIBRARIES}
#    ${QT_QT3SUPPORT_LIBRARY} # is it needed?
    ${GPSIM_LIBRARY} # for GPSIM support
	)

install(TARGETS ktechlab_qt DESTINATION bin)

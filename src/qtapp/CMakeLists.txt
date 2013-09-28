
# for qt only executable

set( ktechlab_qt_SRCS
	mainqt.cpp
	mainwindowqt.cpp
	)

set ( ktechlab_qt_HEADERS_TO_MOC
	mainwindowqt.h
	)

qt4_wrap_cpp(ktechlab_qt_MOCED_HEADERS ${ktechlab_qt_HEADERS_TO_MOC})

include(${QT_USE_FILE})

add_definitions(${QT_DEFINITIONS})

add_executable(ktechlab_qt
	${ktechlab_qt_SRCS}
	${ktechlab_qt_MOCED_HEADERS}
	)

target_link_libraries(ktechlab_qt
	${QT_LIBRARIES}
	)

install(TARGETS ktechlab_qt DESTINATION bin)

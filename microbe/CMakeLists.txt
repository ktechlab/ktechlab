project(microbe)

set( VERSION 0.3 )
cmake_minimum_required(VERSION 2.6)

find_package( KDE4 4.2.0 REQUIRED )
find_package( Qt4 REQUIRED )

add_definitions(${QT4_DEFINITIONS})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDES}
	${KDE4_INCLUDES}
)

set( microbe_SRCS
	main.cpp
)

qt4_automoc( ${microbe_SRCS} )

add_executable( microbe ${microbe_SRCS} )
target_link_libraries( microbe
	${QT_LIBRARIES}
	${KDE4_LIBRARIES}
)


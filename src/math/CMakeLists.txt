include_directories( ${CMAKE_CURRENT_SRC} ${CMAKE_CURRENT_BUILD} )

set( math_SRCS
	matrix.cpp
	qvector.cpp
	qmatrix.cpp
)

kde4_add_library( ktlmath SHARED ${math_SRCS} )
target_link_libraries( ktlmath
	m
)
install(TARGETS ktlmath ${INSTALL_TARGETS_DEFAULT_ARGS} )

include_directories( ${CMAKE_CURRENT_SRC} ${CMAKE_CURRENT_BUILD} )

set( math_SRCS
	matrix.cpp
	quickvector.cpp
	quickmatrix.cpp
)

if(KDEVPLATFORM_ENABLED)

kde4_add_library( ktlmath SHARED ${math_SRCS} )
target_link_libraries( ktlmath
	LINK_PUBLIC m
)
install(TARGETS ktlmath ${INSTALL_TARGETS_DEFAULT_ARGS} )

endif(KDEVPLATFORM_ENABLED)

add_library( ktlmath_static STATIC ${math_SRCS} )
target_link_libraries( ktlmath_static LINK_PUBLIC m )

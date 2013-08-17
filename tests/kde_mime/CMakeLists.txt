
set( KDE_MIME_SRCS kde_mime_test.cpp )

kde4_add_executable( ktechlab_kde_mime_test ${KDE_MIME_SRCS} )

target_link_libraries( ktechlab_kde_mime_test ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} )

# install( TARGETS ktechlab_kde_mime_test  ${INSTALL_TARGETS_DEFAULT_ARGS} )

include_directories( ${KDEVPLATFORM_INCLUDE_DIR} )

set( ktlinterfaces_LIB_SRCS
   idocumentplugin.cpp
   )

kde4_add_library( ktlinterfaces SHARED ${ktlinterfaces_LIB_SRCS} )

target_link_libraries( ktlinterfaces
   ${KDEVPLATFORM_INTERFACES_LIBRARY}
   )

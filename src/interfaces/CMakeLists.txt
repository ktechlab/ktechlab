include_directories( ${KDEVPLATFORM_INCLUDE_DIR} )

set( ktlinterfaces_LIB_SRCS
   idocumentplugin.cpp
   component/icomponentplugin.cpp
   component/componentmodel.cpp
   component/componentmimedata.cpp
   )

kde4_add_library( ktlinterfaces SHARED ${ktlinterfaces_LIB_SRCS} )

target_link_libraries( ktlinterfaces
   ${KDEVPLATFORM_INTERFACES_LIBRARY}
   )

install( TARGETS ktlinterfaces ${INSTALL_TARGETS_DEFAULT_ARGS} )


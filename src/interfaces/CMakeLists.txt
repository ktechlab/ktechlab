include_directories( ${KDEVPLATFORM_INCLUDE_DIR} )

set( ktlinterfaces_LIB_SRCS
   component/connector.cpp
   idocumentscene.cpp
   iconrouter.cpp
   idocumentmodel.cpp
   datacontainer.cpp
   idocumentplugin.cpp
   component/icomponentplugin.cpp
   component/icomponent.cpp
   component/componentmodel.cpp
   component/componentcontainer.cpp
   component/componentmimedata.cpp
   component/connectoritem.cpp
   component/node.cpp
   )

kde4_add_library( ktlinterfaces SHARED ${ktlinterfaces_LIB_SRCS} )

target_link_libraries( ktlinterfaces
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   )

install( TARGETS ktlinterfaces ${INSTALL_TARGETS_DEFAULT_ARGS} )


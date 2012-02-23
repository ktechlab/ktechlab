include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    )

set( ktlinterfaces_LIB_SRCS
   component/idocumentitem.cpp
   component/connector.cpp
   idocumentscene.cpp
   irouterplugin.cpp
   iroutinginformation.cpp
   idocumentmodel.cpp
   idocumentplugin.cpp
   icomponentdocument.cpp
   private/documentitem.cpp
   component/icomponentplugin.cpp
   component/icomponent.cpp
   component/componentmodel.cpp
   component/componentmimedata.cpp
   component/connectoritem.cpp
   component/icomponentitem.cpp
   component/node.cpp

   simulator/ielement.cpp
   simulator/ielementfactory.cpp
   simulator/isimulator.cpp
   simulator/isimulatorfactory.cpp
   simulator/isimulationmanager.cpp
   )

kde4_add_library( ktlinterfaces SHARED ${ktlinterfaces_LIB_SRCS} )

target_link_libraries( ktlinterfaces
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_SUBLIME_LIBRARIES}
   )

install( TARGETS ktlinterfaces ${INSTALL_TARGETS_DEFAULT_ARGS} )

include_directories( ${KDEVPLATFORM_INCLUDE_DIR} )

set( ktlinterfaces_LIB_SRCS 
   idocumentmodel.cpp
   idocumentplugin.cpp
   icomponentdocument.cpp
   component/icomponentplugin.cpp
   component/icomponent.cpp
   component/componentmodel.cpp
   component/componentmimedata.cpp
   simulator/ipin.cpp 
   simulator/ielement.cpp
   simulator/ielementset.cpp 
   simulator/ielementfactory.cpp
   simulator/isimulator.cpp
   simulator/isimulatorfactory.cpp
   simulator/isimulationmanager.cpp
   simulator/iwire.cpp
   simulator/genericelementfactory.cpp
   )

kde4_add_library( ktlinterfaces SHARED ${ktlinterfaces_LIB_SRCS} )

target_link_libraries( ktlinterfaces
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_SUBLIME_LIBRARIES}
   )

install( TARGETS ktlinterfaces ${INSTALL_TARGETS_DEFAULT_ARGS} )

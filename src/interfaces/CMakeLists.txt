include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    )

set( ktlinterfaces_LIB_SRCS
   component/idocumentitem.cpp
   component/connector.cpp
   iktlplugin.cpp
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

   simulator/ipin.cpp
   simulator/ielement.cpp
   simulator/ielementset.cpp
   simulator/ielementfactory.cpp
   simulator/isimulator.cpp
   simulator/isimulatorfactory.cpp
   simulator/isimulationmanager.cpp
   simulator/iwire.cpp
   )

set ( ktlinterfaces_qt_HEADERS_TO_MOC
   component/idocumentitem.h
   component/connector.h
   idocumentscene.h
   iktlplugin.h
   irouterplugin.h
   iroutinginformation.h
   idocumentmodel.h
   idocumentplugin.h
   icomponentdocument.h
   private/documentitem.h
   component/icomponentplugin.h
   component/icomponent.h
   component/componentmodel.h
   component/componentmimedata.h
   component/connectoritem.h
   component/icomponentitem.h
   component/node.h

   simulator/ipin.h
   simulator/ielement.h
   simulator/ielementset.h
   simulator/ielementfactory.h
   simulator/isimulator.h
   simulator/isimulatorfactory.h
   simulator/isimulationmanager.h
   simulator/iwire.h

   )

qt4_wrap_cpp(ktlinterfaces_qt_MOCED_HEADERS ${ktlinterfaces_qt_HEADERS_TO_MOC})

add_library(ktlinterfaces_qt STATIC ${ktlinterfaces_LIB_SRCS} ${ktlinterfaces_qt_MOCED_HEADERS} )
set_target_properties(ktlinterfaces_qt PROPERTIES COMPILE_FLAGS "-DKDE_ENABLED=0")

kde4_add_library( ktlinterfaces SHARED ${ktlinterfaces_LIB_SRCS} )
set_target_properties(ktlinterfaces PROPERTIES COMPILE_FLAGS "-DKDE_ENABLED=1")

target_link_libraries( ktlinterfaces
   PUBLIC
   ${KDEVPLATFORM_INTERFACES_LIBRARIES}
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   ${KDEVPLATFORM_SUBLIME_LIBRARIES}
   )

install( TARGETS ktlinterfaces ${INSTALL_TARGETS_DEFAULT_ARGS} )

include_directories(
   ${KDE4_INCLUDES}
   ${CMAKE_SOURCE_DIR}/src
)

set(document_engine_SRCS
   documentengine.cpp
)

kde4_add_plugin(plasma_engine_document
   ${document_engine_SRCS}
)

target_link_libraries(plasma_engine_document
   ${KDE4_KDECORE_LIBS}
   ${KDE4_PLASMA_LIBS}
   ktlshell
)

install(TARGETS plasma_engine_document
   DESTINATION ${PLUGIN_INSTALL_DIR}
)

install(FILES plasma-engine-ktechlab-document.desktop
   DESTINATION ${SERVICES_INSTALL_DIR}
)

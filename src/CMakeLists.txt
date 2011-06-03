if(KDEVPLATFORM_ENABLED)

add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(gui)
add_subdirectory(interfaces)
add_subdirectory(lib)
add_subdirectory(plugins)
add_subdirectory(kdevplugins)

#add_subdirectory(context)
#add_subdirectory(flowparts)
#add_subdirectory(micro)
#add_subdirectory(mechanics)
#add_subdirectory(electronics)
#add_subdirectory(languages)
#add_subdirectory(drawparts)
#add_subdirectory(contexthelp)
#add_subdirectory(math)

else(KDEVPLATFORM_ENABLED)

# no KDE4, build just the libs
add_subdirectory(lib)

endif(KDEVPLATFORM_ENABLED)

if(KDEVPLATFORM_ENABLED)

include_directories(
    ${KDE4_INCLUDES}
    ${QT_INCLUDES}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/drawparts
    ${CMAKE_SOURCE_DIR}/src/electronics
    ${CMAKE_SOURCE_DIR}/src/electronics/components
    ${CMAKE_SOURCE_DIR}/src/electronics/simulation
    ${CMAKE_SOURCE_DIR}/src/flowparts
    ${CMAKE_SOURCE_DIR}/src/gui
    ${CMAKE_SOURCE_DIR}/src/gui/itemeditor
    ${CMAKE_SOURCE_DIR}/src/languages
    ${CMAKE_SOURCE_DIR}/src/mechanics
    ${CMAKE_SOURCE_DIR}/src/micro
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

#####################################################################
# DBUS
#####################################################################
set(dbus_SRCS
#    dbus/RootDBusHandler.cpp
)

#####################################################################
# LIBKTECHLAB
#####################################################################
set(ktechlablib_LIB_SRCS
    ${dbus_SRCS}
)

########### next target ###############

SET(ktechlab_SRCS
    ktechlabideextension.cpp
    mainwindow.cpp
    main.cpp
    simulationmanager.cpp
)

#kde4_automoc( ${ktechlab_SRCS} )

#kde4_add_library(ktechlablib SHARED ${ktechlablib_LIB_SRCS})
#target_link_libraries(ktechlablib
#    ${KDE4_KUTILS_LIBS}
#    ${KDE4_KDEUI_LIBS}
#)

kde4_add_executable(ktechlab ${ktechlab_SRCS})

target_link_libraries( ktechlab
   ${KDE4_KDEUI_LIBS}
   ${KDE4_KIO_LIBS}
   ${KDEVPLATFORM_SHELL_LIBRARIES}
   # ktechlablib
   ktlinterfaces
   gui
#  languages gui micro flowparts
#  mechanics electronics elements components drawparts
#  itemeditor math khtml kutils ${GPSIM_LIBRARY} ktexteditor
   kio plasma
#  kdeprint
)

install(TARGETS ktechlab DESTINATION bin)


########### install files ###############

install(FILES  microbe.xml DESTINATION share/apps/katepart/syntax)
install(FILES
    ktechlabrc
    ktechlabui.rc
    ktechlabcircuitui.rc
    ktechlabitemviewui.rc
    ktechlabmechanicsui.rc
    ktechlabtextui.rc
    ktechlabkateui.rc

    DESTINATION ${DATA_INSTALL_DIR}/ktechlab
)
install(FILES  ktechlab.desktop DESTINATION share/applnk/Development)

# XML mime types
set( SHARED_MIME_INFO_MINIMUM_VERSION "0.30" )
find_package( SharedMimeInfo )
if( SHARED_MIME_INFO_FOUND )
    install(FILES ktechlab.xml DESTINATION ${XDG_MIME_INSTALL_DIR} )
    update_xdg_mimetypes( ${XDG_MIME_INSTALL_DIR} )
endif( SHARED_MIME_INFO_FOUND )

endif(KDEVPLATFORM_ENABLED)


#add_subdirectory(core)
#add_subdirectory(gui)
#add_subdirectory(flowparts)
#add_subdirectory(micro)
#add_subdirectory(mechanics)
#add_subdirectory(electronics)
#add_subdirectory(languages)
#add_subdirectory(drawparts)
#add_subdirectory(contexthelp)
#add_subdirectory(math)

#include_directories(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/drawparts ${CMAKE_SOURCE_DIR}/src/electronics ${CMAKE_SOURCE_DIR}/src/electronics/components ${CMAKE_SOURCE_DIR}/src/electronics/simulation ${CMAKE_SOURCE_DIR}/src/flowparts ${CMAKE_SOURCE_DIR}/src/gui ${CMAKE_SOURCE_DIR}/src/gui/itemeditor ${CMAKE_SOURCE_DIR}/src/languages ${CMAKE_SOURCE_DIR}/src/mechanics ${CMAKE_SOURCE_DIR}/src/micro ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${KDE3_INCLUDE_DIR} ${QT_INCLUDE_DIR} )


########### next target ###############

SET(ktechlab_SRCS
    ktechlab.cpp
    main.cpp
)

#kde4_automoc( ${ktechlab_SRCS} )

kde4_add_executable(ktechlab ${ktechlab_SRCS})

target_link_libraries( ktechlab ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS}#core gui micro flowparts
#	mechanics electronics elements components languages drawparts
#	itemeditor math khtml kutils ${GPSIM_LIBRARY} ktexteditor kio
#	kdeprint )
)

install(TARGETS ktechlab DESTINATION bin)


########### install files ###############

install(FILES  microbe.xml DESTINATION share/apps/katepart/syntax)
install(FILES
    ktechlabui.rc
    ktechlabcircuitui.rc
    ktechlabflowcodeui.rc
    ktechlabitemviewui.rc
    ktechlabmechanicsui.rc
    ktechlabtextui.rc
    ktechlabkateui.rc

    DESTINATION share/apps/ktechlab)
install(FILES  ktechlab.desktop DESTINATION share/applnk/Development)
install(FILES
    x-circuit.desktop
    x-flowcode.desktop
    x-ktechlab.desktop
    x-microbe.desktop

    DESTINATION share/mimelink/application)

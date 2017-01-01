include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(itemeditor_STAT_SRCS
   itemeditor.cpp
   orientationwidget.cpp
   propertyeditor.cpp
   propertyeditorfile.cpp
   propertyeditorinput.cpp
   propertyeditoritem.cpp
   propertyeditorlist.cpp
   propertysubeditor.cpp
   propertyeditorcolor.cpp
   componentmodelwidget.cpp
)

kde4_add_library(itemeditor STATIC ${itemeditor_STAT_SRCS})
target_link_libraries( itemeditor electronics )

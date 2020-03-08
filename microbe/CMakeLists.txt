SET(microbe_SRCS
   btreebase.cpp
   btreenode.cpp
   main.cpp
   traverser.cpp
   expression.cpp
   pic14.cpp
   variable.cpp
   optimizer.cpp
   instruction.cpp
   microbe.cpp
   parser.cpp
)

#qt4_automoc(${microbe_SRCS})

add_executable(microbe ${microbe_SRCS})

target_link_libraries(microbe
    KF5::CoreAddons
    KF5::I18n

    Qt5::Core

    pthread)

install(TARGETS microbe ${INSTALL_TARGETS_DEFAULT_ARGS})

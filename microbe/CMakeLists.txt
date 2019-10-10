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

# https://stackoverflow.com/questions/10984442/how-to-detect-c11-support-of-a-compiler-with-cmake/20165220#20165220
set_property(TARGET microbe PROPERTY CXX_STANDARD 11)
set_property(TARGET microbe PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(microbe
    KF5::KDELibs4Support
    KF5::CoreAddons
    Qt5::Core
    pthread)

install(TARGETS microbe ${INSTALL_TARGETS_DEFAULT_ARGS})

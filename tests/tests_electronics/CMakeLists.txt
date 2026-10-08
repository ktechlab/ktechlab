set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/)

include_directories(
    ${SRC_DIR}  # needed for subdirs
    ${SRC_DIR}/core
    ${CMAKE_BINARY_DIR}/src/core  # for the kcfg file
    ${SRC_DIR}/drawparts
    ${SRC_DIR}/electronics
    ${SRC_DIR}/electronics/components
    ${SRC_DIR}/electronics/simulation
    ${SRC_DIR}/flowparts
    ${SRC_DIR}/gui
    ${CMAKE_BINARY_DIR}/src/gui  # for ui-generated files
    ${SRC_DIR}/gui/itemeditor
    ${SRC_DIR}/languages
    ${SRC_DIR}/mechanics
    ${SRC_DIR}/micro
)
enable_testing(true)

add_compile_options("--coverage")
add_link_options("--coverage")

add_executable(test_electronics test_electronics.cpp)

target_link_libraries( test_electronics
    PRIVATE
    test_ktechlab
    Qt6::Test
)

# Needed for CI to find and run the test
add_test(NAME test_electronics COMMAND test_electronics)


include_directories(
    ${KDEVPLATFORM_INCLUDE_DIR}
    ${CMAKE_CURRENT_BUILD_DIR}
)

kde4_add_unit_test( basictest basictest.cpp )

target_link_libraries( basictest
    ${QT_QTTEST_LIBRARY}
    ${KDEVPLATFORM_TESTS_LIBRARIES}
)

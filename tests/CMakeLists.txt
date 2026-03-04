add_executable(wbuffer_tests
    # cases/base_buffer.cpp
    cases/extended.cpp
)

target_link_libraries(wbuffer_tests
    PRIVATE
    GTest::gtest_main
    WBuffer
)

target_include_directories(wbuffer_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/include
)

include(GoogleTest)
gtest_discover_tests(wbuffer_tests)

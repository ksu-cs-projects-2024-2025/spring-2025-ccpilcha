add_executable(chunk_renderer_tests
  chunk_renderer.test.cpp
)

# Link to core_testing (which has UNIT_TESTING defined)
target_link_libraries(chunk_renderer_tests
  PRIVATE
    core_testing
    GTest::gtest
    GTest::gtest_main
)

# Optional: link STB if needed in your tests
target_include_directories(chunk_renderer_tests PRIVATE ${Stb_INCLUDE_DIR})

# Register test with CTest
add_test(NAME ChunkRendererTests COMMAND chunk_renderer_tests)

add_executable(LoggerTests logger_tests.cpp)

target_link_libraries(LoggerTests GTest::gtest_main LoggerLib target_flags)

include(GoogleTest)
gtest_discover_tests(LoggerTests)
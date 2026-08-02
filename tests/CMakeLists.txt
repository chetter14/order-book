add_executable(IntegrationTests integration_tests.cpp)

target_link_libraries(IntegrationTests GTest::gtest_main 
                    OrderBookLib OrderGeneratorLib LoggerLib target_flags)

include(GoogleTest)
gtest_discover_tests(IntegrationTests)
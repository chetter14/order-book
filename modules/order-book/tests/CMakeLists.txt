add_executable(OrderBookTests order_book_tests.cpp)

target_link_libraries(OrderBookTests GTest::gtest_main OrderBookLib target_flags)

include(GoogleTest)
gtest_discover_tests(OrderBookTests)
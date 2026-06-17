enable_testing()

add_executable(OrderBookTests order_book_tests.cpp)

target_include_directories(OrderBookTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(OrderBookTests GTest::gtest_main OrderBookLib)

include(GoogleTest)
gtest_discover_tests(OrderBookTests)
add_executable(OrderGeneratorTests order_generator_tests.cpp)

target_include_directories(OrderGeneratorTests PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../order-book/include)

target_link_libraries(OrderGeneratorTests GTest::gtest_main OrderGeneratorLib OrderBookLib target_flags)

include(GoogleTest)
gtest_discover_tests(OrderGeneratorTests)
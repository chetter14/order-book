add_library(OrderBookLib STATIC order_book.cpp)

target_include_directories(OrderBookLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_library(OrderBookLib STATIC order_book.cpp)

target_include_directories(OrderBookLib PUBLIC 
                    ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(OrderBookLib PRIVATE target_flags)
add_library(OrderGeneratorLib STATIC order_generator.cpp)

target_include_directories(OrderGeneratorLib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(OrderGeneratorLib PUBLIC OrderBookLib PRIVATE target_flags)
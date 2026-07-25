add_library(OrderGeneratorLib STATIC order_generator.cpp)

target_include_directories(OrderGeneratorLib PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../order-book/include)

target_link_libraries(OrderGeneratorLib PRIVATE target_flags)
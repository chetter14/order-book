add_executable(OrderBookApp main.cpp)

target_include_directories(OrderBookApp PUBLIC 
                    {CMAKE_CURRENT_SOURCE_DIR}/../modules/order-book/include
                    {CMAKE_CURRENT_SOURCE_DIR}/../modules/order-generator/include
                    {CMAKE_CURRENT_SOURCE_DIR}/../modules/logger/include)

target_link_libraries(OrderBookApp target_flags OrderBookLib OrderGeneratorLib LoggerLib)
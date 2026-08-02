add_library(LoggerLib STATIC logger.cpp)

target_include_directories(LoggerLib PUBLIC 
                        ${CMAKE_CURRENT_SOURCE_DIR}/../include
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../order-book/include)

target_link_libraries(LoggerLib PRIVATE target_flags)
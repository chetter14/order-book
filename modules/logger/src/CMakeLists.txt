add_library(LoggerLib STATIC logger.cpp)

target_include_directories(LoggerLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(LoggerLib PUBLIC OrderBookLib PRIVATE target_flags)
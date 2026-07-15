cmake_minimum_required(VERSION 3.26)

project(OrderBook CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

find_package(GTest REQUIRED)

option(USE_ASAN "Address + UB sanitizers" OFF)
option(USE_TSAN "Thread sanitizer" OFF)
option(USE_COVERAGE "Produce code coverage" OFF)

# Set of flags used by all the targets
add_library(target_flags INTERFACE)

# MSVC and other compilers' warnings
if (MSVC)
    target_compile_options(target_flags INTERFACE /W4 /permissive-)
else()
    target_compile_options(target_flags INTERFACE -Wall -Wextra -Wpedantic)
endif()

if (USE_ASAN AND USE_TSAN)
    message(FATAL_ERROR "Can't use ASAN and TSAN together!")
endif()

if (NOT MSVC)
    if (USE_ASAN)
        target_compile_options(target_flags INTERFACE -fsanitize=address,undefined -g)
        target_link_options(target_flags INTERFACE -fsanitize=address,undefined)
    endif()
    if (USE_TSAN)
        target_compile_options(target_flags INTERFACE -fsanitize=thread -g)
        target_link_options(target_flags INTERFACE -fsanitize=thread)
    endif()
    if (USE_COVERAGE)
        target_compile_options(target_flags INTERFACE --coverage -O0 -g)
        target_link_options(target_flags INTERFACE --coverage)
    endif()
endif()

add_subdirectory(apps)
add_subdirectory(modules)

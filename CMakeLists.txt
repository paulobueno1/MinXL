cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_options(-Wno-gnu-anonymous-struct -Wno-nested-anon-types)
add_compile_options(-Wno-return-type-c-linkage)

add_library(
    MinXL
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Array.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/String.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Variant.cpp
)

set_target_properties(MinXL PROPERTIES OUTPUT_NAME "minxl")

target_include_directories(MinXL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
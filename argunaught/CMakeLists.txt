cmake_minimum_required(VERSION 3.10)
project(argunaught CXX)
set (CMAKE_CXX_STANDARD 17)

add_library(
    argunaught
    src/command_group.cpp
    src/command.cpp
    src/formatting.cpp
    src/option_list.cpp
    src/parse_result.cpp
    src/parser.cpp
    src/sub_parser.cpp
)

target_include_directories(argunaught PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
cmake_minimum_required(VERSION 3.10)
project(argunaught CXX)
set (CMAKE_CXX_STANDARD 17)

add_library(
    argunaught
    src/argunaught.cpp
)

target_include_directories(argunaught PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
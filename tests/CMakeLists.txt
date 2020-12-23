cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 17)

add_executable(
    basic_test
    test.cpp
)

target_link_libraries(
    basic_test 
    argunaught
  )

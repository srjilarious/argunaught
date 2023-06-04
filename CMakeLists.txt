cmake_minimum_required(VERSION 3.23)
project(argunaught VERSION 1.3.0 LANGUAGES CXX)

add_subdirectory(argunaught)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()

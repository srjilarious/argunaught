cmake_minimum_required(VERSION 3.23)
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

if(MSVC)
    target_compile_options(argunaught PRIVATE /W4)
else()
    target_compile_options(argunaught PRIVATE -Wall -Wextra)
endif()

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/argunaught)

target_sources(
    argunaught
    PUBLIC
      FILE_SET public_headers
      TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
      FILES
      ${HEADER_DIR}/argunaught.hpp
      ${HEADER_DIR}/formatting.hpp
      ${HEADER_DIR}/forward_decl.hpp
)

target_include_directories(argunaught PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS argunaught
    FILE_SET public_headers
)

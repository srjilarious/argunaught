cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_STANDARD 17)

add_executable(
    basic_example
    basic_example.cpp
)

target_link_libraries(
    basic_example
    argunaught
  )

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                   "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES Catch2/2.11.1@catchorg/stable
             BASIC_SETUP)

add_executable(unit_tests unit_tests.cpp)
target_link_libraries(unit_tests argunaught ${CONAN_LIBS})
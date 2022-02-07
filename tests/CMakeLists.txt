cmake_minimum_required(VERSION 3.10)

set(PROG_OUTPUT_DIR "${CMAKE_BINARY_DIR}")

add_executable(
    basic_example
    basic_example.cpp
)

target_link_libraries(
    basic_example
    argunaught
  )

set_target_properties(
    basic_example PROPERTIES
    CXX_STANDARD 17
    CMAKE_CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROG_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROG_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROG_OUTPUT_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROG_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROG_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROG_OUTPUT_DIR}
  )

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                   "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES catch2/2.13.8
             BASIC_SETUP)

add_executable(unit_tests unit_tests.cpp)
target_link_libraries(unit_tests argunaught ${CONAN_LIBS})

set_target_properties(
    unit_tests PROPERTIES
    CXX_STANDARD 17
    CMAKE_CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROG_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROG_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROG_OUTPUT_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROG_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROG_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROG_OUTPUT_DIR}
  )
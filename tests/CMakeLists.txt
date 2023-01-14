cmake_minimum_required(VERSION 3.10)

set(PROG_OUTPUT_DIR "${CMAKE_BINARY_DIR}")

# ------------------------------------------------------------------------
add_executable(
    quick_example
    quick_example.cpp
)

target_link_libraries(
    quick_example
    argunaught
  )

set_target_properties(
    quick_example PROPERTIES
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

# ------------------------------------------------------------------------
add_executable(
    larger_example
    larger_example.cpp
)

target_link_libraries(
    larger_example
    argunaught
  )

set_target_properties(
    larger_example PROPERTIES
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

# ------------------------------------------------------------------------
# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                   "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES catch2/2.13.8
             BASIC_SETUP)

add_executable(unit_tests 
    unit/unit_tests.cpp
    unit/command_tests.cpp
    unit/group_tests.cpp
    unit/options_tests.cpp
    unit/positional_args_tests.cpp
    unit/sub_parser_tests.cpp
    unit/word_wrap_tests.cpp
  )

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
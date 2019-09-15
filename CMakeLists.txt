cmake_minimum_required(VERSION 3.5)

project(Genex)

set (CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE include/)
target_sources(${PROJECT_NAME} INTERFACE
    include/generationally_indexed_container.hpp)

enable_testing()


add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Build and then run all the tests.")

add_subdirectory("tests")

cmake_minimum_required(VERSION 3.5)
find_package(Boost)

project(Genex)

set (CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE include/)
target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    ${Boost_INCLUDE_DIR})

file(GLOB target_source_files "include/*")
foreach(target_source_file IN LISTS target_source_files)
    target_sources(${PROJECT_NAME} INTERFACE ${target_source_file})
endforeach()

enable_testing()


add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Build and then run all the tests.")

add_subdirectory("tests")

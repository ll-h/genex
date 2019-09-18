add_custom_target(tests COMMENT "Run all the unit tests.")

function(add_test_and_dependency name)
    add_test(
        NAME "${name}"
        COMMAND "${name}")
    add_dependencies(tests "${name}")
endfunction()

function(target_name_from_file target_name file_name)
    string(REGEX REPLACE ".*/" "" base_name ${file_name})
    string(REPLACE ".cpp" "" base_name "${base_name}")
    set(${target_name} "${base_name}" PARENT_SCOPE)
endfunction()

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB test_files "*.cpp")

foreach(test_file IN LISTS test_files)
    target_name_from_file(target "${test_file}")
    message(STATUS "${target}")
    add_executable("${target}" EXCLUDE_FROM_ALL "${test_file}")
    add_test_and_dependency("${target}")
endforeach()

add_dependencies(check tests)

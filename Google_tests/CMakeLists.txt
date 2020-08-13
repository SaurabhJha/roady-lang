project(Google_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Google_Tests_run regular_expression_test.cc finite_automaton_test.cc)
target_link_libraries(Google_Tests_run tokenizer)
target_link_libraries(Google_Tests_run gtest gtest_main)

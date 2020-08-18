cmake_minimum_required(VERSION 3.15)

project(Google_tests)

add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TOKENIZER_TEST_FILES
        tokenizer_tests/finite_automaton_test.cc
        tokenizer_tests/regular_expression_test.cc
        tokenizer_tests/tokenizer_test.cc)
add_executable(Google_Tests_run ${TOKENIZER_TEST_FILES})
target_link_libraries(Google_Tests_run utils)
target_link_libraries(Google_Tests_run tokenizer)
target_link_libraries(Google_Tests_run gtest gtest_main)

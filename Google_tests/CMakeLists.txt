cmake_minimum_required(VERSION 3.15)

project(Google_tests)

add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TEST_FILES
        tokenizer_tests/finite_automaton_test.cc
        tokenizer_tests/regular_expression_test.cc
        tokenizer_tests/tokenizer_test.cc
        parser_tests/grammar_test.cc
        parser_tests/parser_test.cc
        ast_tests/syntax_tree_test.cc)
set(SOURCE_FILES
        ../tokenizer/finite_automaton.cc
        ../tokenizer/regular_expression.cc
        ../tokenizer/tokenizer.cc
        ../parser/grammar.cc
        ../parser/parser.cc
        ../ast/syntax_tree.cc)
set(HEADER_FILES
        ../tokenizer/finite_automaton.h
        ../tokenizer/regular_expression.h
        ../tokenizer/tokenizer.h
        ../parser/grammar.h
        ../parser/parser.h
        ../ast/syntax_tree.h)
add_executable(Google_Tests_run ${TEST_FILES} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(Google_Tests_run gtest gtest_main)

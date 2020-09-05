cmake_minimum_required(VERSION 3.15)

project(roady_lang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(.)
set(SOURCE_FILES
        tokenizer/finite_automaton.cc
        tokenizer/regular_expression.cc
        tokenizer/tokenizer.cc
        parser/grammar.cc
        parser/parser.cc
        ast/syntax_tree.cc
        main.cc)
set(HEADER_FILES
        tokenizer/finite_automaton.h
        tokenizer/regular_expression.h
        tokenizer/tokenizer.h
        parser/grammar.h
        parser/parser.h
        ast/syntax_tree.h)
add_executable(roady_lang ${SOURCE_FILES} ${HEADER_FILES})
add_subdirectory(Google_tests)

cmake_minimum_required(VERSION 3.15)

project(roady_lang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(.)

set(TOKENIZER_SOURCE_FILES
        tokenizer/finite_automaton.cc
        tokenizer/regular_expression.cc
        tokenizer/tokenizer.cc)
set(TOKENIZER_HEADER_FILES
        tokenizer/finite_automaton.h
        tokenizer/regular_expression.h
        tokenizer/tokenizer.h)
set(PARSER_SOURCE_FILES
        parser/grammar.cc
        parser/parser.cc)
set(PARSER_HEADER_FILES
        parser/grammar.h
        parser/parser.h)

add_executable(tokenize ${TOKENIZER_SOURCE_FILES} ${PARSER_SOURCE_FILES} ${TOKENIZER_HEADER_FILES} ${PARSER_HEADER_FILES}
        tokenizer_main.cc)

add_executable(parse ${TOKENIZER_SOURCE_FILES} ${PARSER_SOURCE_FILES} ${TOKENIZER_HEADER_FILES} ${PARSER_HEADER_FILES}
        parser_main.cc)

set(AST_SOURCE_FILES ast/syntax_tree.cc)
set(AST_HEADER_FILES ast/syntax_tree.h)

add_executable(construct_ast
        ${TOKENIZER_SOURCE_FILES} ${PARSER_SOURCE_FILES} ${TOKENIZER_HEADER_FILES}
        ${PARSER_HEADER_FILES} ${AST_SOURCE_FILES} ${AST_HEADER_FILES} ast_main.cc)

add_subdirectory(Google_tests)

project(tokenizer)

include_directories(.)
set(HEADER_FILES finite_automaton.h regular_expression.h tokenizer.h)
set(SOURCE_FILES finite_automaton.cc regular_expression.cc tokenizer.cc)

add_library(tokenizer STATIC ${SOURCE_FILES} ${HEADER_FILES})

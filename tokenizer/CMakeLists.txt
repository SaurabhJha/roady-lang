project(tokenizer)

include_directories(.)

set(HEADER_FILES regular_expression.h finite_automaton.h)
set(SOURCE_FILES regular_expression.cc finite_automaton.cc)

add_library(tokenizer STATIC ${SOURCE_FILES} ${HEADER_FILES})
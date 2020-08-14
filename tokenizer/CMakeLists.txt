project(tokenizer)

include_directories(.)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(HEADER_FILES regular_expression.h finite_automaton.h tokenizer.h)
set(SOURCE_FILES regular_expression.cc finite_automaton.cc tokenizer.cc)

add_library(tokenizer STATIC ${SOURCE_FILES} ${HEADER_FILES})
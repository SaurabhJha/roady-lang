cmake_minimum_required(VERSION 3.15)
project(roady_lang)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

set(SOURCE_FILES main.cc tokenizer/common.cc tokenizer/regex.cc tokenizer/finite_automata.cc tokenizer/tokenizer.cc
        parser/grammar.cc)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(roady_lang ${SOURCE_FILES})

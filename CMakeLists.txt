cmake_minimum_required(VERSION 3.15)
project(roady_lang)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

set(SOURCE_FILES main.cc tokeniser/common.cc tokeniser/regex.cc tokeniser/finite_automata.cc)

add_executable(roady_lang ${SOURCE_FILES})

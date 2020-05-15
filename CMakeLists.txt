cmake_minimum_required(VERSION 3.15)
project(roady_lang)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

set(SOURCE_FILES lexical_analyzer.cc parser_common.cc parser_top_down.cc parser_bottom_up.cc main.cc)

add_executable(roady_lang ${SOURCE_FILES})

cmake_minimum_required(VERSION 3.15)

project(roady_lang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(.)
add_executable(roady_lang main.cc)
add_subdirectory(tokenizer)
target_link_libraries(roady_lang tokenizer)
add_subdirectory(Google_tests)

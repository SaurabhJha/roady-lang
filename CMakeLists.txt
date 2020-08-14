cmake_minimum_required(VERSION 3.15)
project(roady_lang)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)
set(SOURCE_FILES main.cc)

include_directories(tokenizer)
add_subdirectory(tokenizer)
add_executable(roady_lang ${SOURCE_FILES})
target_link_libraries(roady_lang tokenizer)

add_subdirectory(Google_tests)

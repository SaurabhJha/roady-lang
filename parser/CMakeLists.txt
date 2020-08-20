cmake_minimum_required(VERSION 3.15)

project(parser)

include_directories(.)
set(HEADER_FILES grammar.h)
set(SOURCE_FILES grammar.cc)

add_library(parser STATIC ${SOURCE_FILES} ${HEADER_FILES})

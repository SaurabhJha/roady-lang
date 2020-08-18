cmake_minimum_required(VERSION 3.15)

project(utils)

include_directories(.)
set(HEADER_FILES set_utils.h)
set(SOURCE_FILES set_utils.cc)

add_library(utils STATIC ${SOURCE_FILES} ${HEADER_FILES})

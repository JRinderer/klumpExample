cmake_minimum_required(VERSION 3.8)
project(ParserEx)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp error.cpp scanner.cpp scanner.h error.h assembler.h assembler.cpp tables.cpp tables.h)
add_executable(ParserEx ${SOURCE_FILES} tables.cpp tables.h)
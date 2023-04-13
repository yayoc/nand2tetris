cmake_minimum_required(VERSION 3.15)
project(vm_translator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} main.cpp parser.cpp code_writer.cpp)

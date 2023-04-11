cmake_minimum_required(VERSION 3.14)
project(vm_translator)

set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} main.cpp parser.cpp code_writer.cpp)

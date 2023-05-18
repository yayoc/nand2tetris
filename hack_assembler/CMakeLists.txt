cmake_minimum_required(VERSION 3.14)
project(hack_assembler)

add_executable(${PROJECT_NAME} main.cpp hack_assembler.cpp parser.cpp code.cpp symbol_table.cpp)

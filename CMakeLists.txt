cmake_minimum_required(VERSION 3.10)

project(TuringMachine_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS
    exceptions.h
    iterator.h
    option.h
    dynamic_array.h
    sequence.h
    array_sequence.h
    igenerator_rule.h
    generator.h
    lazy_rules.h
    lazy_sequence.h
    turing_tape.h
    turing_machine.h
)

add_executable(TuringMachine main.cpp ${HEADERS})
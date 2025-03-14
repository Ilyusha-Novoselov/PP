cmake_minimum_required(VERSION 3.10)

project(Labs)

set(CMAKE_GENERATOR_PLATFORM "x64")

set(LABS Lab_1 Lab_2)

foreach(LAB ${LABS})
    add_subdirectory(${LAB})
endforeach()
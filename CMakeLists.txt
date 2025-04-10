cmake_minimum_required(VERSION 3.10)

project(Labs)

set(LABS Lab_1 Lab_2 Lab_3 Lab_4)

foreach(LAB ${LABS})
    add_subdirectory(${LAB})
endforeach()
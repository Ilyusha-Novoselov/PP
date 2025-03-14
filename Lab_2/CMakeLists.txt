cmake_minimum_required(VERSION 3.10)

project(MyProject)

set(COMMON_SOURCE "${CMAKE_SOURCE_DIR}/Table/Table.cxx")
set(COMMON_HEADER "${CMAKE_SOURCE_DIR}/Table/")

file(GLOB CPP_FILES "*.cxx")

foreach(CPP_FILE ${CPP_FILES})
    get_filename_component(EXECUTABLE_NAME ${CPP_FILE} NAME_WE)

    add_executable(${EXECUTABLE_NAME} ${CPP_FILE} ${COMMON_SOURCE})

    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${COMMON_HEADER})
endforeach()
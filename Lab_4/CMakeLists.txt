cmake_minimum_required(VERSION 3.10)

project(MyProject)

find_package(MPI REQUIRED)

file(GLOB CPP_FILES "*.cxx")

foreach(CPP_FILE ${CPP_FILES})
    get_filename_component(EXECUTABLE_NAME ${CPP_FILE} NAME_WE)

    add_executable(${EXECUTABLE_NAME} ${CPP_FILE} ${COMMON_SOURCE})

    target_link_libraries(${EXECUTABLE_NAME} PRIVATE MPI::MPI_CXX)
endforeach()
file(GLOB LAB2_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx")
file(GLOB LAB2_INC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hxx")
set(LAB2_INC_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/inc")

add_executable(Lab_2 ${LAB2_SRC_FILES} ${LAB2_INC_FILES})

target_include_directories(Lab_2 PRIVATE ${LAB2_INC_DIRS})

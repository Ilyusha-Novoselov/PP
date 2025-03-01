macro(copy_file _src _dst)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_src} ${_dst}
        RESULT_VARIABLE _res
        OUTPUT_VARIABLE _out
        ERROR_VARIABLE  _err
    )
    if(_res)
        message(FATAL_ERROR "ERROR ---> could not be copied: ${_err}")
    endif()
endmacro()

if(WIN32)

get_property(PTHREADS_INCLUDE_DIR DIRECTORY PROPERTY PTHREADS_INCLUDE_DIR)
get_property(PTHREADS_LIB_DIR DIRECTORY PROPERTY PTHREADS_LIB_DIR)
get_property(PTHREADS_DLL_DIR DIRECTORY PROPERTY PTHREADS_DLL_DIR)

endif()

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx")
file(GLOB INC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hxx")
set(INC_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/inc")

if(WIN32)
set(PTHREADS_LIBRARIES "${PTHREADS_LIB_DIR}/pthreadVC2.lib" CACHE PATH "Path to pthreads LIB file" FORCE)
set(PTHREADS_DLL "${PTHREADS_DLL_DIR}/pthreadVC2.dll" CACHE PATH "Path to pthreads DLL file" FORCE)
endif()

set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../build/bin")

file(MAKE_DIRECTORY ${BIN_DIR})

add_executable(Lab_1 ${SRC_FILES} ${INC_FILES})

if(WIN32)

target_include_directories(Lab_1 PRIVATE ${INC_DIRS} ${PTHREADS_INCLUDE_DIR})

target_link_directories(Lab_1 PRIVATE ${PTHREADS_LIB_DIR})

target_link_libraries(Lab_1 ${PTHREADS_LIBRARIES})
copy_file("${PTHREADS_DLL}" "${BIN_DIR}/pthreadVC2.dll")

else()

target_include_directories(Lab_1 PRIVATE ${INC_DIRS})

endif()




set_target_properties(Lab_1 PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
)


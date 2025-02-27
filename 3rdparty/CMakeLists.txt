# Указываем пути к заголовочным файлам и библиотекам pthreads
set(PTHREADS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pthreads/include" CACHE PATH "Path to pthreads include directory")
set(PTHREADS_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pthreads/lib/x64" CACHE PATH "Path to pthreads library directory")
set(PTHREADS_DLL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/pthreads/dll/x64" CACHE PATH "Path to pthreads library directory")

# Экспортируем переменные для использования в поддиректориях
set_property(DIRECTORY PROPERTY PTHREADS_INCLUDE_DIR ${PTHREADS_INCLUDE_DIR})
set_property(DIRECTORY PROPERTY PTHREADS_LIB_DIR ${PTHREADS_LIB_DIR})
set_property(DIRECTORY PROPERTY PTHREADS_LIBRARIES ${PTHREADS_DLL_DIR})
cmake_minimum_required(VERSION 3.10)

# Устанавливаем имя проекта
project(Labs)

# Указываем, что мы собираем только для x64 архитектуры
set(CMAKE_GENERATOR_PLATFORM "x64")

if(WIN32)
	# Включаем CMakeLists.txt из папки 3rdparty/pthreads
	add_subdirectory(3rdparty)
endif()

# Определяем список лабораторных работ
set(LABS Lab_1)

# Проходим по каждому элементу списка лабораторных работ и включаем их CMakeLists.txt
foreach(LAB ${LABS})
    add_subdirectory(${LAB})
endforeach()
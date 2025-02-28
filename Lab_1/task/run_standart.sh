#!/bin/bash

# Устанавливаем имя выходного файла
OUTPUT="program"

# Компилируем main.cpp с флагами оптимизации и отладочной информацией
g++ -o $OUTPUT main_standart.cpp -Wall -Wextra -O2

# Проверяем, успешно ли прошла компиляция
if [ $? -eq 0 ]; then
    echo "Компиляция успешна! Запускаем $OUTPUT..."
    ./"$OUTPUT"
else
    echo "Ошибка компиляции."
fi

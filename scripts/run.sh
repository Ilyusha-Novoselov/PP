#!/bin/bash

clear_console() {
  clear
}

cd ..

if [ ! -d "build" ]; then
  mkdir build
fi

cd build

if [ ! -f "CMakeCache.txt" ]; then
  cmake ..
fi

cmake --build .

select_executable() {
  local lab_dir="$1"
  
  local executables=()
  for file in "$lab_dir"/*; do
    if [[ -x "$file" && ! -d "$file" ]]; then
      executables+=("$file")
    fi
  done

  clear_console

  echo "Выберите исполняемый файл:"
  for i in "${!executables[@]}"; do
    echo "$((i + 1)): ${executables[$i]}"
  done

  read -p "Введите номер: " choice
  if [[ $choice =~ ^[0-9]+$ && $choice -ge 1 && $choice -le ${#executables[@]} ]]; then
    selected_executable="${executables[$((choice - 1))]}"
    ./"$selected_executable"
  else
    echo "Неверный выбор"
    sleep 2
  fi
}

clear_console

echo "Выберите лабораторную работу:"
labs=("Lab_1" "Lab_2")
for i in "${!labs[@]}"; do
  echo "$((i + 1)): ${labs[$i]}"
done

read -p "Введите номер: " lab_choice
if [[ $lab_choice =~ ^[0-9]+$ && $lab_choice -ge 1 && $lab_choice -le ${#labs[@]} ]]; then
  selected_lab="${labs[$((lab_choice - 1))]}"
  echo "Выбрана лабораторная работа: $selected_lab"

  if [ -d "$selected_lab" ]; then
    select_executable "$selected_lab"
  else
    echo "Директория $selected_lab не найдена"
    sleep 2 
  fi
else
  echo "Неверный выбор"
  sleep 2
fi

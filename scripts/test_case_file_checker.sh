#!/bin/bash

TEST_DIR="test_directory"
mkdir -p "$TEST_DIR/subdir"

# Функция для создания большого файла с заданным текстом
create_large_file() {
    local file_path="$1"
    local size_mb="$2"
    local text="$3"
    
    local lines=$((size_mb * 1024))  # Исправленный расчет количества строк

    for ((i=0; i<lines; i++)); do
        echo -e "$text" >> "$file_path"
    done
}

# Создаем большие текстовые файлы (100MB)
echo -e "Hello world\nHello\nAnother hello\n" > "$TEST_DIR/file1.txt"
create_large_file "$TEST_DIR/file2.txt" 100 "Nothing here\nJust a test\n"
create_large_file "$TEST_DIR/subdir/file3.txt" 100 "Random text\nCompletely unrelated\n"
echo -e "Hello world\nHello\nAnother hello\n" > "$TEST_DIR/subdir/file4.txt"

# Пустой файл
touch "$TEST_DIR/empty.txt"

# Бинарный файл (не должен обрабатываться)
dd if=/dev/urandom of="$TEST_DIR/binary.png" bs=1M count=10

# Файл с похожим названием, но не текстовый
echo "This is not a txt file" > "$TEST_DIR/fake_txt.dat"

# Глубокая вложенность директорий
mkdir -p "$TEST_DIR/deep/nested/folder"

echo -e "Hello\n" > "$TEST_DIR/deep/nested/folder/deepfile.txt"

echo "Test directory with large files created: $TEST_DIR"

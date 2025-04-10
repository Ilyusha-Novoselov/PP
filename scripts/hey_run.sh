#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Использование: $0 <количество_запросов> <количество_соединений>"
    exit 1
fi

REQUESTS=$1   
CONCURRENCY=$2 

if ! command -v hey &> /dev/null; then
    echo "Ошибка: утилита 'hey' не найдена. Установите её с помощью 'sudo snap install hey' или 'go install github.com/rakyll/hey@latest'"
    exit 1
fi

hey -n "$REQUESTS" -c "$CONCURRENCY" http://127.0.0.1:8080/

echo "Нагрузочное тестирование завершено."

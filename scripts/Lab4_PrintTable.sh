#!/bin/bash

process_counts=(1 2 4 8 16)

results_console=()
results_word=()

base_time=""

for np in "${process_counts[@]}"; do
    
    result=$(mpirun -np "$np" --oversubscribe ./../build/Lab_4/Lab4_PoissonEquation)
    
    iterations=$(echo "$result" | grep -oP 'Converged in \K\d+')
    time=$(echo "$result" | grep -oP 'Execution time: \K[\d.]+')
    error=$(echo "$result" | grep -oP 'max error = \K[\d.e-]+')
    
    if [[ -z "$iterations" || -z "$time" || -z "$error" ]]; then
        echo "Ошибка: не удалось распарсить вывод программы для $np процессов"
        exit 1
    fi

    if [[ "$np" -eq 1 ]]; then
        base_time="$time"
    fi

    speedup=$(echo "scale=4; $base_time / $time" | bc)
    efficiency=$(echo "scale=4; $speedup / $np" | bc)

    printf -v line "| %-20s | %-18s | %-15s | %-20s | %-10s | %-10s |\n" \
        "$np" "$time" "$iterations" "$error" "$speedup" "$efficiency"
    results_console+=("$line")
    results_word+=("$np;$time;$iterations;$error;$speedup;$efficiency")
done

echo "=== Console table ==="
printf "+----------------------+--------------------+-----------------+----------------------+------------+------------+\n"
printf "| %-20s | %-18s | %-15s | %-20s | %-10s | %-10s |\n" \
       "Number of Processes" "Execution Time" "Iterations" "Max Error" "SpeedUp" "Efficiency"
printf "+----------------------+--------------------+-----------------+----------------------+------------+------------+\n"

for line in "${results_console[@]}"; do
    printf "%s" "$line"
done

printf "+----------------------+--------------------+-----------------+----------------------+------------+------------+\n"

printf "\n"
echo "=== Word table ==="
echo "Number of Processes;Execution Time (s);Iterations;Max Error;SpeedUp;Efficiency"

for row in "${results_word[@]}"; do
    echo "$row"
done

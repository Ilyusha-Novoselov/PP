#!/bin/bash

process_counts=(1 2 4 8 16)

results_console=()
results_word=()

base_time=""

for np in "${process_counts[@]}"; do
    
    result=$(mpirun -np "$np" --oversubscribe ./../build/Lab_4/Lab4_PoissonEquation)
    mapfile -t lines <<< "$result"

    if [[ "${#lines[@]}" -lt 2 ]]; then
        echo "Ошибка: недостаточно строк в выводе программы для $np процессов"
        exit 1
    fi

    read -r iterations error time <<< "${lines[0]}"

    if [[ -z "$time" ]]; then
        echo "Ошибка: пустое значение времени"
        exit 1
    fi

    comm_times=()
    comp_times=()

    for ((i = 1; i <= np; i++)); do
        read -r _ comm comp <<< "${lines[i]}"
        comm_times+=("$comm")
        comp_times+=("$comp")
    done

    if [[ "$np" -eq 1 ]]; then
        base_time="$time"
    fi

    speedup=$(echo "scale=4; $base_time / $time" | bc 2>/dev/null)
    efficiency=$(echo "scale=4; $speedup / $np" | bc 2>/dev/null)

    comm_inline=$(IFS=' '; echo "${comm_times[*]}")
    comp_inline=$(IFS=' '; echo "${comp_times[*]}")

    comm_multiline=$(printf "%s\n" "${comm_times[@]}")
    comp_multiline=$(printf "%s\n" "${comp_times[@]}")
    IFS=$'\n' read -rd '' -a comm_lines <<< "$comm_multiline"
    IFS=$'\n' read -rd '' -a comp_lines <<< "$comp_multiline"
    max_lines=${#comm_lines[@]}

    printf -v first_row "| %-20s | %-18s | %-15s | %-20s | %-10s | %-10s | %-25s | %-25s |\n" \
        "$np" "$time" "$iterations" "$error" "$speedup" "$efficiency" "${comm_lines[0]}" "${comp_lines[0]}"
    results_console+=("$first_row")

    for ((j = 1; j < max_lines; j++)); do
        printf -v row "| %-20s | %-18s | %-15s | %-20s | %-10s | %-10s | %-25s | %-25s |\n" \
            "" "" "" "" "" "" "${comm_lines[j]}" "${comp_lines[j]}"
        results_console+=("$row")
    done

    printf -v delim "+----------------------+--------------------+-----------------+----------------------+------------+------------+---------------------------+---------------------------+\n"
    results_console+=("$delim")

    results_word+=("$np;$time;$iterations;$error;$speedup;$efficiency;$comm_inline;$comp_inline")
done

echo "=== Console table ==="
printf "+----------------------+--------------------+-----------------+----------------------+------------+------------+---------------------------+---------------------------+\n"
printf "| %-20s | %-18s | %-15s | %-20s | %-10s | %-10s | %-25s | %-25s |\n" \
       "Number of Processes" "Execution Time" "Iterations" "Max Error" "SpeedUp" "Efficiency" "Comm Times" "Comp Times"
printf "+----------------------+--------------------+-----------------+----------------------+------------+------------+---------------------------+---------------------------+\n"

for line in "${results_console[@]}"; do
    printf "%s" "$line"
done

echo -e "\n=== Word table ==="
echo "Number of Processes;Execution Time (s);Iterations;Max Error;SpeedUp;Efficiency;Comm Times;Comp Times"
for row in "${results_word[@]}"; do
    echo "$row"
done
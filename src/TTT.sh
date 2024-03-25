for threads in 1 2 4 8 10; do  # Adjust as needed if you want to include more thread counts
    export OMP_NUM_THREADS=$threads
    for size in 20 100 1000; do
        echo "Running: ./main $size $size 1 ../CSVs/output_${size}_${threads}.csv $threads"
        ./main $size $size 1 ../CSVs/output_${size}_${threads}.csv $threads
    done
done
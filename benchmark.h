#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#define BENCHMARK_FILE_NAME "benchmark.csv"

void benchmark_save(const char *algorithm_name, int thread_count, int prime_count, uintmax_t time) {
    FILE *benchmark_file;
    bool new_file = true;

    benchmark_file = fopen(BENCHMARK_FILE_NAME, "r");
    if (benchmark_file != NULL) {
        new_file = false;
        fclose(benchmark_file);
    }

    benchmark_file = fopen("benchmark.csv", "a");
    if (new_file) {
        fprintf(benchmark_file, "algorith_name, thread_count, prime_count, nanosseconds\n");
    }
    
    fprintf(benchmark_file, "%s, %d, %d, %"PRIuMAX"\n", algorithm_name, thread_count, prime_count, time);
    fclose(benchmark_file);
}
#endif

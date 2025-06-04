#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "buffer.h"
#include "timer.h"
#include "benchmark.h"

#define MAX_BOUND 1000000
#define UNITS 4

typedef struct {
    int start;
    int end;
} Partition;

typedef struct {
    int *content;
    int count;
    int capacity;
} IntBuffer;

IntBuffer search_primes(int start, int end) {
    IntBuffer buffer = {0};

    if (start < 2) {
        start = 2;
    }

    for (int number = start; number <= end; number++) {
        bool prime = true;

        for (int test = 2; test * test <= number; test++) {
            if (number % test == 0) {
                prime = false;
                break;
            }
        }

        if (prime) {
            bf_append(buffer, number);
        }
    }
    return buffer;
}

Partition *new_partition(int start, int end) {
    Partition *p = (Partition*) malloc(sizeof(Partition));
    p->start = start;
    p->end = end;
    return p;
}

void *unit_prime_search(void *arg) {
    Partition partition = *((Partition*)arg);
    free(arg);

    IntBuffer primes = search_primes(partition.start, partition.end);

    IntBuffer *output = (IntBuffer*)malloc(sizeof(IntBuffer));
    memcpy(output, &primes, sizeof(IntBuffer));
    return output;
}

typedef struct {
    IntBuffer *content;
    int count;
    int capacity;
} ResultBuffer;

int main() {
    pthread_t threads[UNITS];
    const int partition_size = MAX_BOUND / UNITS;

    Timer timer = {0};
    timer_start(&timer);
    
    for (int i = 0; i < UNITS; i++) {
        int start = i * partition_size;
        int end = start + partition_size;

        if (i == UNITS - 1) {
            end += MAX_BOUND - UNITS * partition_size;
        }

        int error = pthread_create(
                &threads[i], 
                NULL,
                unit_prime_search,
                new_partition(start, end)
            );

        if (error != 0) {
            fprintf(stderr, "ERROR: failed to start work unit\n");
            return 1;
        }
    }

    ResultBuffer results = {0};
    bf_resize(results, UNITS);

    for (int i = 0; i < UNITS; i++) {
        void *result;
        if (pthread_join(threads[i], &result)) {
            fprintf(stderr, "ERROR: failed to join work unit\n");
            return 1;
        }

        IntBuffer primes = *((IntBuffer*) result);
        bf_append(results, primes);
       
        free(result);
    }

    int prime_count = 0;
    uintmax_t elapsed_time = timer_end(&timer);

    printf("=== PRIMES ===\n");
    for (int i = 0; i < results.count; i++) {
        IntBuffer primes = results.content[i];
        for (int j = 0; j < primes.count; j++) {
            prime_count++;
            printf("%d\n", primes.content[j]);
        }
        bf_free(primes);
    }

    printf("=== STATS ===\n");
    printf("Elasped time: "); print_duration(elapsed_time); putchar('\n');
    printf("Total primes: %d\n", prime_count);

    benchmark_save("basic-primes", UNITS, prime_count, elapsed_time);
    bf_free(results);
    return 0;
}

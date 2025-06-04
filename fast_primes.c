#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "buffer.h"
#include "timer.h"
#include "benchmark.h"

#define MAX_BOUND 1000000
#define CORES 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int num;
    int start;
    int end;
} Partition;

typedef struct {
    Partition *content;
    int count;
    int capacity;
    int head;
} PartitionQueue;

PartitionQueue queue;

Partition* queue_pop() {
    pthread_mutex_lock(&mutex);

    if (queue.head >= queue.count) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    int index = queue.head++;

    pthread_mutex_unlock(&mutex);
    return &queue.content[index];
}

typedef struct {
    int *content;
    int count;
    int capacity;
} IntBuffer;

IntBuffer prime_search(int start, int end) {
    IntBuffer buffer = {0};

    if (start < 2) {
        start = 2;
    }

    for (int number = start; number <= end; number++) {
        bool is_prime = true;

        for (int pos = 0; pos < buffer.count; pos++) {
            int prime = buffer.content[pos];
            
            if (prime * prime > number) {
                break;
            }
            
            if (number % prime == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) {
            bf_append(buffer, number);
        }
    }
    return buffer;
}

IntBuffer prime_cache = {0};

IntBuffer cached_prime_search(int start, int end) {
    IntBuffer buffer = {0};

    if (start < 2) {
        start = 2;
    }

    for (int number = start; number <= end; number++) {
        bool is_prime = true;

        for (int pos = 0; pos < prime_cache.count; pos++) {
            int prime = prime_cache.content[pos];
            
            if (prime * prime > number) {
                break;
            }
            
            if (number % prime == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) {
            bf_append(buffer, number);
        }
    }
    return buffer;
}

IntBuffer *results;

void *unit_prime_search(void *arg) {
    while (true) {
        Partition *p = queue_pop();
        if (p == NULL) {
            break;
        }

        IntBuffer primes = cached_prime_search(p->start, p->end);
        results[p->num] = primes;

        printf("Partition %d finished, total primes: %d\n", p->num, primes.count);
    }
    return NULL;
}

int main(int argc, const char **argv) {
    int bound = 1000000;
    if (argc >= 2) {
        bound = atoi(argv[1]);
    }

    Timer timer = {0};
    timer_start(&timer);
    
    results = (IntBuffer*) malloc(sizeof(IntBuffer) * (bound / 1000 + 2));

    prime_cache = prime_search(0, 1000);
    results[0] = prime_cache;

    for (int i = 1; i < bound / 1000; i++) {
        Partition p;
        p.num = i;

        p.start = i * 1000;
        p.end = (i + 1) * 1000;

        bf_append(queue, p);
    }

    if (bound % 1000 != 0) {
        Partition p;
        p.num = bound / 1000 + 1;

        p.start = bound - (bound % 1000);
        p.end = bound;
        bf_append(queue, p);
    }

    pthread_t threads[CORES];

    for (int i = 0; i < CORES; i++) {
        int error = pthread_create(&threads[i], NULL, unit_prime_search, NULL);
        if (error != 0) {
            fprintf(stderr, "ERROR: failed to start work unit\n");
            exit(1);
        }
    }

    for (int i = 0; i < CORES; i++) {
        int error = pthread_join(threads[i], NULL);
        if (error != 0) {
            fprintf(stderr, "ERROR: failed to join work unit\n");
            exit(1);
        }
    }

    int prime_count = 0;
    uintmax_t elapsed_time = timer_end(&timer);

    printf("=== PRIMES ===\n");
    for (int i = 0; i < (bound / 1000 + 2); i++) {
        IntBuffer primes = results[i];
        if (bound < 1000 && i == 0) {
            continue;
        }

        for (int j = 0; j < primes.count; j++) {
            prime_count++;
            printf("%d\n", primes.content[j]);
        }
        bf_free(primes);
    }

    printf("=== STATS ===\n");
    printf("Elasped time: "); print_duration(elapsed_time); putchar('\n');
    printf("Total primes: %d\n", prime_count);

    printf("bound: %d\n", bound);

    benchmark_save("primes-aritmetics", CORES, prime_count, elapsed_time);
    return 0;
}

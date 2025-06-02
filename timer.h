#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <inttypes.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uintmax_t start_time;
    uintmax_t end_time;
} Timer;

uintmax_t timer_get_time() {
    struct timespec tp;
    int error = clock_gettime(CLOCK_MONOTONIC, &tp);
    if (error != 0) {
        fprintf(stderr, "ERROR: failed to resolve current time\n");
        exit(1);
    }
    return tp.tv_sec * (1000000000L) + tp.tv_nsec;
}

uintmax_t timer_start(Timer *timer) {
    timer->start_time = timer_get_time();
    return timer->start_time;
}

uintmax_t timer_end(Timer *timer) {
    timer->end_time = timer_get_time();
    return timer->end_time - timer->start_time;
}

void print_duration(uintmax_t duration) {
    uintmax_t seconds = duration / (1000L * 1000L * 1000L);
    duration %= (1000L * 1000L * 1000L);
    
    if (seconds != 0) {
       printf("%"PRIuMAX"s ", seconds); 
    }

    uintmax_t millisecs = duration / (1000 * 1000);
    duration %= (1000 * 1000);

    if (millisecs != 0) {
        printf("%"PRIuMAX"ms ", millisecs);
    }
    printf("%"PRIuMAX"ns", duration);
}
#endif

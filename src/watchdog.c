#include "watchdog.h"

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>

#define NUMBER_OF_THREADS 3
#define WATCHDOG_TIMEOUT 1
#define WATCHDOG_THREAD_TIMEOUT 2

bool watchdog_threads[NUMBER_OF_THREADS] = { 0 };
pthread_mutex_t watchdog_mutex = PTHREAD_MUTEX_INITIALIZER;

void inform_watchdog(int idx) {
    assert(idx < NUMBER_OF_THREADS);

    pthread_mutex_lock(&watchdog_mutex);
    watchdog_threads[idx] = true;
    pthread_mutex_unlock(&watchdog_mutex);
}

void watchdog(void) {
    long times_since_last_update[NUMBER_OF_THREADS] = { 0 };

    // Set current times
    for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
        times_since_last_update[i] = time(0);
    }

    while(1) {
        sleep(WATCHDOG_TIMEOUT);

        pthread_mutex_lock(&watchdog_mutex);
        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            if (watchdog_threads[i]) {
                times_since_last_update[i] = time(0);
                watchdog_threads[i] = false;
            } else if (time(0) - times_since_last_update[i] > WATCHDOG_THREAD_TIMEOUT) {
                printf("OMG! THE THREAD IS NOT RESPONDING 0_0\n");
            }
        }
        pthread_mutex_unlock(&watchdog_mutex);

        printf("Watchdog watching dogs... or something...\n");
    }
}
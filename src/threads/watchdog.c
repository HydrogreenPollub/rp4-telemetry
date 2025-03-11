#include <threads/watchdog.h>

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define NUMBER_OF_THREADS 2
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
                // TODO Maybe restart the thread instead of crashing entire app 0_0
                raise(SIGTERM);
            }
        }
        pthread_mutex_unlock(&watchdog_mutex);
    }
}
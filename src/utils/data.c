#include <utils/data.h>

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

static pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;

struct TSData td = { 0 };

void set_x(int8_t x) {
    pthread_mutex_lock(&data_mutex);
    td.x = x;
    pthread_mutex_unlock(&data_mutex);
}

void *read_data(void) {
    return (void *)&td;
}
#ifndef DATA_H
#define DATA_H

#include <pthread.h>

struct TelemetryData {
    int x;
    int y;
    pthread_mutex_t lock;    
};

void update_x(int x);
void update_y(int y);
char *read_data();

#endif
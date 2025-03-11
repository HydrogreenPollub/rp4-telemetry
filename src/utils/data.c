#include <utils/data.h>

struct TelemetryData td = {
    .x = -1,
    .y = -1,
    .lock = PTHREAD_MUTEX_INITIALIZER
 };

void update_x(int x) {
    pthread_mutex_lock(&td.lock);
    td.x = x;
    pthread_mutex_unlock(&td.lock);
}

void update_y(int y) {
    pthread_mutex_lock(&td.lock);
    td.y = y;
    pthread_mutex_unlock(&td.lock);
}

// TODO add flatbuffer support and maybe change logic
char *read_data() {
    pthread_mutex_lock(&td.lock);

    char *buffer = malloc(255);
    if (buffer) {
        snprintf(buffer, 255, "X: %d, Y: %d\r", td.x, td.y);
    }

    pthread_mutex_unlock(&td.lock);
    return buffer;
}
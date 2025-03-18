#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdint.h>

struct TSData {
    int8_t x;
    int8_t y;
};

void set_x(int8_t x);
void *read_data(void);

#endif
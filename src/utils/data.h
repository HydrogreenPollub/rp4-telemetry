#ifndef DATA_H
#define DATA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#include <utils/proto/capnp_c.h>
#include <utils/proto/ts_data.capnp.h>

#define TSDATA_BUFFER_SIZE 1024

void set_x(int8_t x);
const uint8_t *read_data(void);

#endif
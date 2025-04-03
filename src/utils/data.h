#ifndef DATA_H
#define DATA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#include <utils/proto/capnp_c.h>
#include <utils/proto/ts_data.capnp.h>

#define TSDATA_BUFFER_SIZE 4096

// Provides setter prototypes for all of the fields
#define XSET_TSDATA(FIELD, TYPE) void set_##FIELD(TYPE value);
#include <utils/ts_data.def>
#undef XSET_TSDATA

const uint8_t *read_data(void);

#endif
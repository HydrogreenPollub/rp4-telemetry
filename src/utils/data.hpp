#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <mutex>
#include <vector>
#include <string>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <utils/capnp-proto/ts_data.capnp.h>

#define TSDATA_BUFFER_SIZE 144

void init_data(void);
const uint8_t* read_data(void);
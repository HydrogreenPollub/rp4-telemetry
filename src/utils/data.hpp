#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <mutex>
#include <vector>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <utils/capnp-proto/ts_data.capnp.h>

#define TSDATA_BUFFER_SIZE 128

void init_data(void);
std::vector<uint8_t> read_data(void);
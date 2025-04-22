#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <utils/data.h>
#include <utils/serial.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <fcntl.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>

void* lora(void* arg);

#ifdef __cplusplus
}
#endif

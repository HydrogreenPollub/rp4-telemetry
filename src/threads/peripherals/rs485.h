#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <utils/serial.h>

#include <assert.h>
#include <errno.h>
#include <gpiod.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void* rs485(void* arg);

#ifdef __cplusplus
}
#endif

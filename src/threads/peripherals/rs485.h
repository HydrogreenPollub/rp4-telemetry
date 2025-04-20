#ifndef RS485_H
#define RS485_H

#include <utils/serial.h>

#include <assert.h>
#include <errno.h>
#include <gpiod.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void* rs485(void* arg);

#endif

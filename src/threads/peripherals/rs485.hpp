#pragma once

#include <utils/serial_port.hpp>

#include <thread>

#include <assert.h>
#include <errno.h>
#include <gpiod.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* rs485(void* arg);

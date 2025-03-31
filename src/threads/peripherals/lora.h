#ifndef LORA_H
#define LORA_H

#include <threads/watchdog.h>
#include <utils/data.h>
#include <utils/serial.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <termios.h>

void *lora(void *arg);

#endif
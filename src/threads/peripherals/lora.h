#ifndef LORA_H
#define LORA_H

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

#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int serial_get_device(char* device_file, int baudrate);

#ifdef __cplusplus
}
#endif

#ifndef SERIAL_H
#define SERIAL_H

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

int serial_get_device(char *device_file, int baudrate);

#endif

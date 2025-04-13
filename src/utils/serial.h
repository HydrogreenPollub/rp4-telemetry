#ifndef SERIAL_H
#define SERIAL_H

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int serial_get_device(char* device_file, int baudrate);

#endif

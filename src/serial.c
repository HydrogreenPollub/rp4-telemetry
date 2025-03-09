#include "serial.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>

int serial_get_device(char *device_file, int baudrate) {
    // Maybe add flags as an argument to function
    fprintf(stdout, "SERIAL: Opening device %s with baud rate %d\n", device_file, baudrate);

    int device = open(device_file, O_RDWR | O_NDELAY | O_NONBLOCK);
    assert(device > 0); 
    
    struct termios tty;

    // Get current config
    tcgetattr(device, &tty);

    // Configure serial interface
    tty.c_iflag = IGNPAR | IGNBRK;
    tty.c_oflag = 0;
    tty.c_cflag = CS8 | CREAD | CLOCAL;
    tty.c_lflag = 0;

    // Set baudrate
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    // Save changes
    tcflush(device, TCIOFLUSH);
    tcsetattr(device, TCSANOW, &tty);
    
    fprintf(stdout, "SERIAL: Device descriptor is %d\n", device);

    return device;
}
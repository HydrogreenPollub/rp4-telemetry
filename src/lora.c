#include "lora.h"
#include "watchdog.h"
#include "serial.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

#define LORA_DEVICE "/dev/ttyS0"

void *lora(void *arg) {
    int lora_port = serial_get_device(LORA_DEVICE, B9600);
    assert(lora_port > 0);

    while(true) {
        write(lora_port, "Hello world\r", 12);

        inform_watchdog((int)arg);

        sleep(1);
    }
}
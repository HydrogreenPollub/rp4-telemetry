#include <threads/watchdog.h>
#include <threads/peripherals/lora.h>
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

#define LORA_DEVICE "/dev/ttyS0"

void *lora(void *arg) {
    int lora_port = serial_get_device(LORA_DEVICE, B9600);
    assert(lora_port > 0);

    while(true) {
        const void *data = read_data();
        write(lora_port, data, 1024);

        inform_watchdog(*(uint8_t *)arg);

        sleep(1);
    }
}
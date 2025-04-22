#include <threads/peripherals/lora.h>

#define LORA_DEVICE "/dev/ttyS0"

void* lora(void* arg)
{
    int lora_port = serial_get_device(LORA_DEVICE, B9600);
    assert(lora_port > 0);

    char buffer[TSDATA_BUFFER_SIZE + 4];

    while (true) {
        const void* data = read_data();

        // TODO clean this up :'(
        buffer[0] = 0xFF;
        memcpy(buffer + 1, data, TSDATA_BUFFER_SIZE);
        buffer[TSDATA_BUFFER_SIZE] = 0xEE;
        buffer[TSDATA_BUFFER_SIZE + 1] = '\n';
        buffer[TSDATA_BUFFER_SIZE + 2] = '\r';

        printf("LORA: Sending buffer - ");
        for (int i = 0; i < TSDATA_BUFFER_SIZE; ++i) {
            printf("%02X ", ((unsigned char*)data)[i]);
        }
        printf("\n");

        write(lora_port, buffer, TSDATA_BUFFER_SIZE + 4);

        sleep(5);
    }
}

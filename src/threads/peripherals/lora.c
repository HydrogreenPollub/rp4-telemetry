#include <threads/peripherals/lora.h>

#define LORA_DEVICE "/dev/ttyS0"

void *lora(void *arg) {
    int lora_port = serial_get_device(LORA_DEVICE, B9600);
    assert(lora_port > 0);

    char buffer[TSDATA_BUFFER_SIZE + 2];

    while(true) {
        const void *data = read_data();

        memcpy(buffer, data, TSDATA_BUFFER_SIZE);
        buffer[TSDATA_BUFFER_SIZE] = '\n';
        buffer[TSDATA_BUFFER_SIZE + 1] = '\r';

        // data is not null terminated
        fprintf(stdout, "LORA: Sending buffer - ");
        fwrite(data, 1, TSDATA_BUFFER_SIZE, stdout);
        fprintf(stdout, "\n");
        
        write(lora_port, buffer, TSDATA_BUFFER_SIZE + 2);

        sleep(5);
    }
}
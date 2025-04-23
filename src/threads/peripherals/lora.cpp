#include "data.h"
#include <thread>
#include <threads/peripherals/lora.hpp>

#define LORA_DEVICE "/dev/ttyS0"
constexpr size_t FRAME_SIZE = TSDATA_BUFFER_SIZE + 4;

void* lora(void* arg)
{
    (void)arg;

    SerialPort port(LORA_DEVICE, B9600);
    std::vector<uint8_t> frame(FRAME_SIZE);

    while (true) {
        const uint8_t* data = read_data();

        // Frame structure: [0xFF][data][0xEE]
        frame[0] = 0xFF;
        std::copy(data, data + TSDATA_BUFFER_SIZE, frame.begin() + 1);
        frame[TSDATA_BUFFER_SIZE + 1] = 0xEE;
        frame[TSDATA_BUFFER_SIZE + 2] = '\n';
        frame[TSDATA_BUFFER_SIZE + 3] = '\r';

        printf("LORA: Sending buffer - ");
        for (int i = 0; i < TSDATA_BUFFER_SIZE; ++i) {
            printf("%02X ", ((unsigned char*)data)[i]);
        }
        printf("\n");

        port.write(std::string(frame.begin(), frame.end())); // TODO figure out why << isn't working

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

#include <threads/peripherals/rs485.hpp>

#define GPIO_CHIP "gpiochip0"
#define RS485_DEVICE "/dev/ttyS1" // TODO check if correct
#define EN_RS485 4

void* rs485(void* arg)
{
    (void)arg;

    // Initialize GPIO
    gpiod::chip chip(GPIO_CHIP);
    if (!chip) {
        throw std::runtime_error("RS485: Failed to open GPIO chip");
    }

    auto line = chip.get_line(EN_RS485);
    if (!line) {
        throw std::runtime_error("RS485: Failed to get line");
    }

    // Pull EN_RS485 DOWN
    line.request({ "en_rs485", gpiod::line_request::DIRECTION_OUTPUT, 0 }, 0);
    std::cout << "RS485: EN_RS485 set to LOW" << std::endl;

    // Open the serial device
    SerialPort port(RS485_DEVICE, B9600);
    std::string buf;

    // TODO maybe add some way to break out of the loop to free the resources...
    while (true) {
        buf = port.read();
        if (!buf.empty()) {
            std::cout << "RS485: Received " << buf << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    // Free resources
    line.release();
    return (void*)EXIT_SUCCESS;
}

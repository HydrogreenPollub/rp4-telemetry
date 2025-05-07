#include <threads/peripherals/rs485.hpp>

#define GPIO_CHIP "/dev/gpiochip0"
#define RS485_DEVICE "/dev/ttyS1" // TODO check if correct
#define EN_RS485 4

void* rs485(void* arg)
{
    (void)arg;

    auto request = ::gpiod::chip(GPIO_CHIP)
                       .prepare_request()
                       .set_consumer("set_rs485_low")
                       .add_line_settings(
                           EN_RS485,
                           ::gpiod::line_settings().set_direction(
                               ::gpiod::line::direction::OUTPUT))
                       .do_request();

    request.set_value(EN_RS485, 0);
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
    return (void*)EXIT_SUCCESS;
}

#include <termios.h>
#include <threads/peripherals/rs485.hpp>

#define GPIO_CHIP "/dev/gpiochip0"
#define RS485_DEVICE "/dev/ttySC0"

#define RS485_TXDEN_1 27
#define RS485_TXDEN_2 22

void* rs485(void* arg)
{
    (void)arg;

    // Setup gpiod
    const ::gpiod::line::offsets line_offsets = { RS485_TXDEN_1, RS485_TXDEN_2 };

    // Set both TXDEN to OUTPUT
    auto request = ::gpiod::chip(GPIO_CHIP)
                       .prepare_request()
                       .set_consumer("rs485-TXDEN")
                       .add_line_settings(
                           line_offsets,
                           ::gpiod::line_settings().set_direction(
                               ::gpiod::line::direction::OUTPUT))
                       .do_request();
    // TXDEN_1 decides if we're reading or writing
    // 0 - Transmit
    // 1 - Receive
    request.set_value(RS485_TXDEN_1, ::gpiod::line::value::ACTIVE);

    // Open the serial device
    SerialPort port(RS485_DEVICE, B921600);
    std::string buf;

    // TODO maybe add some way to break out of the loop to free the resources...
    while (true) {
        buf = port.read();
        if (!buf.empty()) {
            std::cout << "RS485: Received " << buf << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

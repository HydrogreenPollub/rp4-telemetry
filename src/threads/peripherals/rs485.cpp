#include <threads/peripherals/rs485.hpp>

#define GPIO_CHIP "/dev/gpiochip0"
#define RS485_DEVICE "/dev/ttyS0" // TODO check if correct
#define EN_RS485 4

void* rs485(void* arg)
{
    (void)arg;
    int ret;

    // Initialize GPIO
    struct gpiod_chip* chip = gpiod_chip_open(GPIO_CHIP);
    if (!chip) {
        printf("RS485: Failed to open GPIO chip - %s\n", strerror(errno));
        return (void*)EXIT_FAILURE;
    }

    struct gpiod_line* line = gpiod_chip_get_line(chip, EN_RS485);
    if (!line) {
        printf("RS485: Failed to get line - %s\n", strerror(errno));
        gpiod_chip_close(chip);
        return (void*)EXIT_FAILURE;
    }

    // Pull EN_RS485 DOWN
    ret = gpiod_line_request_output(line, "telemetry", 0);
    if (ret < 0) {
        printf("RS485: Requesting line as output failed - %s\n", strerror(errno));
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        return (void*)EXIT_FAILURE;
    }

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
    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return (void*)EXIT_SUCCESS;
}

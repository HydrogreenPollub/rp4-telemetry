#include <utils/serial_port.hpp>

SerialPort::SerialPort(const std::string& device_file, int baudrate)
    : baudrate(baudrate), device_file(device_file)
{
    std::cout << "SERIAL: Opening device " << device_file << " with baudrate " << baudrate << std::endl;

    this->file_descriptor = open(device_file.c_str(), O_RDWR | O_NDELAY | O_NONBLOCK);
    if (this->file_descriptor < 0) {
        throw std::runtime_error("SERIAL: Failed to open serial device: " + this->device_file);
    }

    struct termios tty;

    // Get current config
    tcgetattr(this->file_descriptor, &tty);

    // Configure serial interface
    tty.c_iflag = IGNPAR | IGNBRK;
    tty.c_oflag = 0;
    tty.c_cflag = CS8 | CREAD | CLOCAL;
    tty.c_lflag = 0;

    // Set baudrate
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    // Save changes
    tcflush(this->file_descriptor, TCIOFLUSH);
    tcsetattr(this->file_descriptor, TCSANOW, &tty);

    std::cout << "SERIAL: Device descriptor is " << this->file_descriptor << std::endl;
}

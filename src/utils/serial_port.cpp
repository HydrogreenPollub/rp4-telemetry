#include <utils/serial_port.hpp>

SerialPort::SerialPort(const std::string& device_file, int baudrate)
    : baudrate(baudrate)
    , device_file(device_file)
{
    std::cout << "SERIAL: Opening device " << device_file << " with baudrate "
              << baudrate << std::endl;

    this->file_descriptor = open(device_file.c_str(), O_RDWR);
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

    std::cout << "SERIAL: Device descriptor is " << this->file_descriptor
              << std::endl;
}

void SerialPort::write(const std::string& data)
{
    if (::write(this->file_descriptor, data.c_str(), data.size()) < 0) {
        throw std::runtime_error("SERIAL: Writing to serial device failed");
    }
}

std::string SerialPort::read(size_t max_length)
{
    std::string buffer(max_length, '\0');
    ssize_t bytes_read = ::read(this->file_descriptor, &buffer[0], max_length);

    if (bytes_read < 0) {
        throw std::runtime_error(std::string("SERIAL: read failed: ") + std::strerror(errno));
    }

    buffer.resize(bytes_read);
    return buffer;
}

SerialPort& operator<<(SerialPort& port, const std::string& data)
{
    port.write(data);
    return port;
}

SerialPort& operator>>(SerialPort& port, std::string& data)
{
    data = port.read();
    return port;
}

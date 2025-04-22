#pragma once

#include <string>
#include <iostream>
#include <termios.h>
#include <fcntl.h>

// TODO add << operator for writing and >> operator for reading
class SerialPort {
private:
    int baudrate;
    int file_descriptor;
    std::string device_file;
public:
    SerialPort(const std::string& device_file, int baudrate);
};

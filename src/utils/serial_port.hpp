#pragma once

#include <fcntl.h>
#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <termios.h>
#include <unistd.h>

// TODO fix << operator for writing and >> operator for reading
class SerialPort {
private:
  int baudrate;
  int file_descriptor;
  std::string device_file;

public:
  SerialPort(const std::string &device_file, int baudrate);

  void write(const std::string &data);
  std::string read(size_t max_length = 256);
};

#pragma once

#include <utils/data.h>
#include <utils/serial_port.hpp>

#include <vector>

#include <fcntl.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>

void* lora(void* arg);

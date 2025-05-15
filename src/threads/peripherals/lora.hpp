#pragma once

#include <utils/data.h>
#include <utils/serial_port.hpp>

#include <boost/asio.hpp>

#include <thread>
#include <vector>
#include <stdint.h>
#include <unistd.h>

void* lora(void* arg);

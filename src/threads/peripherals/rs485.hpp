#pragma once

#include <utils/serial_port.hpp>

#include <chrono>
#include <gpiod.hpp>
#include <stdexcept>
#include <thread>

void* rs485(void* arg);

#pragma once

#include <utils/data.h>
#include <utils/rs485_parser/hmi_parser.hpp>

#include <boost/asio.hpp>

#include <chrono>
#include <gpiod.hpp>
#include <iostream>
#include <stdexcept>
#include <thread>

void* rs485(void* arg);

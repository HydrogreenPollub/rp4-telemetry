#pragma once

#include <utils/data.h>
#include <utils/minmea/minmea.h>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <assert.h>
#include <iostream>
#include <stdint.h>
#include <thread>

void* gps(void* arg);
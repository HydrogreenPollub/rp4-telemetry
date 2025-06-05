#pragma once

#include <utils/data.hpp>
#include <utils/minmea/minmea.h>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

#include <assert.h>
#include <ctime>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>
#include <thread>

void* gps(void* arg);
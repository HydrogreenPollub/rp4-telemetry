#pragma once

#include <utils/data.h>
#include <utils/minmea/minmea.h>

#include <boost/asio.hpp>

#include <thread>
#include <iostream>
#include <assert.h>
#include <stdint.h>

void* gps(void* arg);
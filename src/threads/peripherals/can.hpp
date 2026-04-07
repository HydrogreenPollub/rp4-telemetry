#pragma once

#include <utils/can-definition/candef.h>
#include <utils/data.hpp>

#include <atomic>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <thread>

#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

void* can(void* arg);
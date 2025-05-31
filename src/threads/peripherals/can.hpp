#pragma once

#include <utils/can_ids/can_ids.h>
#include <utils/data.hpp>

#include <chrono>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <thread>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

class Can {
private:
    int socket;

    void bind();

public:
    void operator()();
};

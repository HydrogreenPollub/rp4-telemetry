#pragma once

#include <utils/can_ids/can_ids.h>
#include <utils/data.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <thread>

class Can {
private:
    int socket;

    void bind();

public:
    void operator()();
};

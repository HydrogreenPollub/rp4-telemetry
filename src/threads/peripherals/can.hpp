#pragma once

#include <utils/can_ids/can_ids.h>
#include <utils/data.hpp>

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

class Can {
private:
    int socket;

    void bind();

public:
    void operator()();
};

#pragma once

#include <utils/can_ids/can_ids.h>
#include <utils/data.h>

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/ioctl.h>
#include <net/if.h>

class Can {
private:
    int socket;

    void bind();
public:
    void operator()();
};

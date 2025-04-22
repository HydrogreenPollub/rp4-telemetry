#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <utils/can_ids/can_ids.h>
#include <utils/data.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

void* can(void* arg);

#ifdef __cplusplus
}
#endif

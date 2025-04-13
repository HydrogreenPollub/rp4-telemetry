#ifndef RS485_H
#define RS485_H

#include <assert.h>
#include <gpiod.h>
#include <string.h>
#include <unistd.h>

int rs485_connect();
int rs485_disconnect();

#endif

#ifndef RS485_H
#define RS485_H

#include <assert.h>
#include <errno.h>
#include <gpiod.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int rs485_connect();
int rs485_disconnect();

#endif

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

void inform_watchdog(uint8_t idx);
void watchdog(void);

#endif
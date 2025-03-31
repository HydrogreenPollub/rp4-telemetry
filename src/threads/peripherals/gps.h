#ifndef GPS_H
#define GPS_H

#include <threads/watchdog.h>
#include <utils/minmea/minmea.h>
#include <utils/data.h>

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void *gps(void *arg);

#endif
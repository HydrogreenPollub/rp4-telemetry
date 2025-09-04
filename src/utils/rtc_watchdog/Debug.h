/*****************************************************************************
 * | File      	:	Debug.h
 * | Author      :   Waveshare team
 * | Function    :	debug with printf
 * | Info        :
 *----------------
 * |	This version:   V1.0
 * | Date        :   2018-01-11
 * | Info        :   Basic version
 *
 ******************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define USE_DEBUG 1
#if USE_DEBUG
#define DEBUG(__info, ...) printf("Debug : " __info, ##__VA_ARGS__)
#else
#define DEBUG(__info, ...)
#endif

#ifdef __cplusplus
}
#endif
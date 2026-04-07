#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "DEV_Config.h"

#define DS3231_Address 0x68

#define Seconds_Reg 0x00
#define Min_Reg 0x01
#define Hour_Reg 0x02
#define Day_Reg 0x03
#define Date_Reg 0x04
#define Month_Reg 0x05
#define Year_Reg 0x06

void DS3231_SET_Calendar(UWORD Year, UWORD Month, UWORD Date);
void DS3231_Read_Calendar(UWORD* Calendar);

void DS3231_SET_Day(UWORD Day);
UWORD DS3231_Read_Day(void);

void DS3231_SET_Hour_Mode(UWORD mode);
UBYTE DS3231_Read_Hour_Mode(void);

void DS3231_SET_Time(UBYTE Hour, UBYTE Min, UBYTE Sec);
void DS3231_Read_Time(UBYTE* Time);

float DS3231_Read_Temperature(void);

UWORD Read_Year_BCD(void);
void SET_Year_BCD(UWORD Year);
UWORD Read_Month_BCD();
void SET_Month_BCD(UWORD Month);
UWORD Read_Date_BCD();
void SET_Date_BCD(UWORD Date);
void SET_Time_Hour_BCD(UWORD data);
UBYTE Read_Time_Hour_BCD(void);
void SET_Time_Min_BCD(UBYTE data);
UBYTE Read_Time_Min_BCD(void);
void SET_Time_Sec_BCD(UBYTE data);
UBYTE Read_Time_Sec_BCD(void);
void Set_Time_BCD(UBYTE* Time);

#ifdef __cplusplus
}
#endif
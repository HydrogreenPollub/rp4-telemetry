#include "DS3231.h"

void Write_Reg(UBYTE Cmd, UBYTE value)
{
    I2C_Write_Byte( Cmd, value);
    
}
UBYTE Read_Reg(UBYTE Cmd)
{
    return I2C_Read_Byte(Cmd);
}

UWORD BCD_Convert_DEC(UWORD code)
{
    code =(((code & 0xf000)>>12) )*1000  + (((code & 0xf00)>>8) )*100  + (((code & 0xf0)>>4) )*10  + (code & 0x0f);
    return code;
}
    
UWORD DEC_Convert_BCD(UWORD code)
{
    code = code%10 + (((code/10)%10) << 4) + (((code/100)%10) << 8) + (((code/1000)%10) << 12);
    return (code);
}
    
/*
    The year can only be from 0 to 99, and Century can be 0 || 1, depending on how you express it.
    If the 21st century is considered to be 1, then the year represented is from 1900 to 2099.
    If the 21st w||ld is considered to be 0, it means that the year is from 2000 to 2199.
*/
// I have calculated from 2000 here, and it is 0 in the 21st century
    
/******************************************************************************
function:  Read Calendar
parameter:
    Year: Year
    Month: Month
    Date: Date
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
void DS3231_SET_Calendar(UWORD Year, UWORD Month, UWORD Date){
    SET_Year_BCD(DEC_Convert_BCD(Year));
    SET_Month_BCD(DEC_Convert_BCD(Month));
    SET_Date_BCD(DEC_Convert_BCD(Date));
}

/******************************************************************************
function:  Read Calendar
parameter:
    Calendar: 
        The buffer size is at least 3 bytes,
        0: Year
        1: Month
        2: Date
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
void DS3231_Read_Calendar(UWORD *Calendar){
    Calendar[0] = BCD_Convert_DEC(Read_Year_BCD());
    Calendar[1] = BCD_Convert_DEC(Read_Month_BCD());
    Calendar[2] = BCD_Convert_DEC(Read_Date_BCD());
}
        
    //Year_Reg     0x06                            //
/******************************************************************************
function:  Read Year
parameter:
return : 0x2000-0x2199
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
UWORD Read_Year_BCD(void)
{
    UWORD vai = Read_Reg(Month_Reg) & 0x80;
    if(vai == 0x80){
        return Read_Reg(Year_Reg) | (0x21 << 8);
    }else{ 
        return Read_Reg(Year_Reg) | (0x20 << 8);
    }
}

/******************************************************************************
function:  SET Year
parameter:
    Year: 0x2000-0x2199
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
void SET_Year_BCD(UWORD Year){
    if(Year>=0x2100){
        Write_Reg(Year_Reg, Year&0xff);
        
        UWORD vai = Read_Reg(Month_Reg);
        Write_Reg(Month_Reg,  vai | 0x80);
    }else if(Year < 0x2100){
        Write_Reg(Year_Reg, Year & 0xff);
        
        UWORD vai = Read_Reg(Month_Reg);
        Write_Reg(Month_Reg,  vai & 0x7f);
    }
}
    
    //Month_Reg     0x05                            //

/******************************************************************************
function:  Read Month
parameter:
    Month: 0x1-0x12
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
UWORD Read_Month_BCD(){
    return Read_Reg(Month_Reg)&0x1f;
}
    


/******************************************************************************
function:  SET Month
parameter:
    Month: 0x1-0x12
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
void SET_Month_BCD(UWORD Month){
    UWORD vai = Read_Reg(Month_Reg);
    Write_Reg(Month_Reg, (vai & 0x80) | Month);
}
    

    
   //Date_Reg     0x04                            //
/******************************************************************************
function:  Read Date
parameter:
return:  0x1-0x31
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
UWORD Read_Date_BCD(){
    return Read_Reg(Date_Reg)&0x3f;
}
    
/******************************************************************************
function:  SET Date
parameter:
    Date: 0x1-0x31
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
void SET_Date_BCD(UWORD Date){
    Write_Reg(Date_Reg, Date&0x3f);
}

    
    //Day          0x03                            //
/******************************************************************************
function:  SET Day
parameter:
    Date: 1-7
Info:
******************************************************************************/
void DS3231_SET_Day(UWORD Day){
    Write_Reg(Day_Reg, Day&0x07);
}
/******************************************************************************
function:  Read Day
parameter:
return : data: 1-7
Info:
******************************************************************************/
UWORD DS3231_Read_Day(void){
    return Read_Reg(Day_Reg)&0x07;
}



    //Hour         0x02                            //
/******************************************************************************
function:  SET the Hour 
parameter:
    data: 0-24 
Info:
    Automatically distinguish between 12 and 24 hour modes
******************************************************************************/
void SET_Time_Hour_BCD(UWORD data){
    UWORD vai = DS3231_Read_Hour_Mode();
    if(vai == 24){
        Write_Reg(Hour_Reg, data & 0x3F) ;
    }else if(vai == 0 || vai == 1){
        if(data>0x12)
            data = data-0x12;
        Write_Reg(Hour_Reg, data & 0x3F) ;
    }
}

    
/******************************************************************************
function:  SET the Hour Mode
parameter:
  data:
    24: 24-hour system
    1 : 12-hour system  afternoon
    0 : 12-hour system  morning
Info:
******************************************************************************/
void  DS3231_SET_Hour_Mode(UWORD mode){
    UBYTE vai = Read_Reg(Hour_Reg);
    if( mode == 12){
        Write_Reg(Hour_Reg, vai|0x40); //12 hour
    }else if( mode == 24 ){
        Write_Reg(Hour_Reg, vai&0x3f); //24 hour   
    }else{
        printf ("Only supp||ts 12 and 24 /r/n");
        Write_Reg(Hour_Reg, vai&0x3f); //24 hour  
    }        
}

/******************************************************************************
function:  Read the Hour Mode
parameter:
return:
    24: 24-hour system
    1 : 12-hour system  afternoon
    0 : 12-hour system  morning
Info:
******************************************************************************/
UBYTE DS3231_Read_Hour_Mode(void)//
{
    UBYTE vai = Read_Reg(Hour_Reg);
    if((vai&0x40) == 0x40){ //12 hour
        if((vai&0x20) == 0x20)
            return 1;
        else 
            return 0;
    }else {//24 hour
        return 24;
    }
}

/******************************************************************************
function:  Read the Hour register
parameter: 
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
UBYTE Read_Time_Hour_BCD (void)
{
    UBYTE vai = DS3231_Read_Hour_Mode();
    if(vai == 24){
        return Read_Reg(Hour_Reg) & 0x3F;
    }else if(vai == 0 || vai == 1){
        vai = Read_Reg(Hour_Reg) & 0x1F;
        if(vai>0x12){
            vai = vai-0x12;
        }
        return vai;
    }
    return 0;
}

//Min            0x01                               //
/******************************************************************************
function:  Set the Min register
parameter: 
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
void SET_Time_Min_BCD (UBYTE data){
    Write_Reg(Min_Reg, data & 0x7F);
}

/******************************************************************************
function:  Read the Min register
parameter: 
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
UBYTE Read_Time_Min_BCD (void){
    return Read_Reg(Min_Reg)&0x7F;
}

//Sec            0x00                               //
/******************************************************************************
function:  Set the seconds register
parameter: 
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
void SET_Time_Sec_BCD (UBYTE data){
    Write_Reg(Seconds_Reg,data & 0x7F);
}

/******************************************************************************
function:  Read Sec
parameter: 
Info:
    Not recommended
    Hexadecimal data
******************************************************************************/
UBYTE Read_Time_Sec_BCD(void){
    return Read_Reg(Seconds_Reg)&0x7F;
}



//Time          0x02  01  00                        //
/******************************************************************************
function:  Set time
parameter: 
    Hour: Hour
    Min : Min
    Sec : Sec
Info:
    The parameter is decimal
******************************************************************************/
void DS3231_SET_Time(UBYTE Hour, UBYTE Min, UBYTE Sec)//
{
    Hour = DEC_Convert_BCD(Hour);
    Min = DEC_Convert_BCD(Min);
    Sec = DEC_Convert_BCD(Sec);
    
    SET_Time_Sec_BCD(Sec&0x7F);
    SET_Time_Min_BCD(Min&0x7F);
    SET_Time_Hour_BCD(Hour&0x3F);
}

/******************************************************************************
function:  Set time
parameter: 
Info:
    Not recommended
******************************************************************************/
void Set_Time_BCD(UBYTE *Time){
    for(int i=0;i<7;i++){
        Write_Reg(i,Time[i]);
    }
}


/******************************************************************************
function:  Read RTC time
parameter:
    Time:
        The buffer size is at least 3 bytes,
        0: hour
        1 minute
        2: seconds
Info:
    Data decimal retention
******************************************************************************/
void  DS3231_Read_Time(UBYTE *Time){
    Time[0] = BCD_Convert_DEC(Read_Time_Hour_BCD());
    Time[1] = BCD_Convert_DEC(Read_Time_Min_BCD());
    Time[2] = BCD_Convert_DEC(Read_Time_Sec_BCD());
}
            
    //Read_Temperature      0x11 12                      //
/******************************************************************************
function:  Read internal digital temperature count value
parameter:
Info:
    Temperature accuracy of 0.25 degrees Celsius
******************************************************************************/
float DS3231_Read_Temperature(void){
    UBYTE vai = Read_Reg(0x0e);
    Write_Reg(0x0e, vai|0x20);//Activate temperature conversion
    DEV_Delay_ms(100);
    UBYTE vai_H = Read_Reg(0x11);
    UBYTE vai_L = Read_Reg(0x12)>>6;
    return vai_H+vai_L*0.25; //Precision 0.25 Celsius
}

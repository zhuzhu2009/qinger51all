#ifndef __PCF8563_H
#define __PCF8563_H

#include "iic.h"

#define SLAW	0xA2
#define SLAR	0xA3


extern u8	hour;
extern u8 minute;
extern u8 second;	//RTC±‰¡ø
extern u8 day;
extern u8 week;
extern u8 month;
extern u8 year;

void PCF8563_WriteNbyte(u8 addr, u8 *p, u8 number);
void PCF8563_ReadNbyte(u8 addr, u8 *p, u8 number);
void PCF8563_ReadTime(void);
void PCF8563_WriteTime(void);

#endif
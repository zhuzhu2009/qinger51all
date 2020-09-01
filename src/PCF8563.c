#include "PCF8563.h"

u8 hour = 0;
u8 minute = 0;
u8 second = 0;	//RTC±äÁ¿
u8 day = 0;
u8 week = 0;
u8 month = 0;
u8 year = 0;

void PCF8563_WriteNbyte(u8 addr, u8 *p, u8 number)			/*	WordAddress,First Data Address,Byte lenth	*/
                         									//F0=0,right, F0=1,error
{
	IIC_Start();
	IIC_Send_Byte(SLAW);
	
	IIC_Wait_Ack();

	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	
	do
	{
		IIC_Send_Byte(*p);		p++;
		
		IIC_Wait_Ack();
	}
	while(--number);
		
	IIC_Stop();
}


/****************************/
void PCF8563_ReadNbyte(u8 addr, u8 *p, u8 number)		/*	WordAddress,First Data Address,Byte lenth	*/
{
	IIC_Start();
	IIC_Send_Byte(SLAW);
	IIC_Wait_Ack();
	
	IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(SLAR);
	IIC_Wait_Ack();
	do
	{
		if(number != 1)		
		{
			*p = IIC_Read_Byte(1);	p++;
		}
		else
		{
			*p = IIC_Read_Byte(0);	p++;
		}
	}
	while(--number);

	IIC_Stop();
}

void PCF8563_ReadTime(void)
{
	u8	tmp[7];

	PCF8563_ReadNbyte(2, tmp, 7);
	second = ((tmp[0] >> 4) & 0x07) * 10 + (tmp[0] & 0x0f);
	minute = ((tmp[1] >> 4) & 0x07) * 10 + (tmp[1] & 0x0f);
	hour   = ((tmp[2] >> 4) & 0x03) * 10 + (tmp[2] & 0x0f);
	day = ((tmp[3] >> 4) & 0x03) * 10 + (tmp[3] & 0x0f);
	week = tmp[4] & 0x07;
	month = ((tmp[5] >> 4) & 0x01) * 10 + (tmp[5] & 0x0f);
	year = ((tmp[6] >> 4) & 0x0f) * 10 + (tmp[6] & 0x0f);
}

void PCF8563_WriteTime(void)
{
	u8	tmp[7];

	tmp[0] = ((second / 10) << 4) + (second % 10);
	tmp[1] = ((minute / 10) << 4) + (minute % 10);
	tmp[2] = ((hour / 10) << 4) + (hour % 10);
	tmp[3] = ((day / 10) << 4) + (day % 10);
	tmp[4] = ((week / 10) << 4) + (week % 10);
	tmp[5] = ((month / 10) << 4) + (month % 10);
	tmp[6] = ((year / 10) << 4) + (year % 10);
	
	PCF8563_WriteNbyte(2, tmp, 7);
}
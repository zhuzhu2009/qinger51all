#include "iic.h"


void IIC_Init(void)
{
	P3n_standard(1<<4);
	P5n_standard(1);
	
/* 	P6n_standard(1<<1);
/* 	P6n_standard(1);*/
}
/****************************/
void	IIC_Delay(void)	//for normal MCS51,	delay (2 * dly + 4) T, for STC12Cxxxx delay (4 * dly + 10) T
{
	delay_us(4);
}

/****************************/
void IIC_Start(void)               //start the IIC, SDA High-to-low when SCL is high
{
	SDA = 1;
	IIC_Delay();
	SCL = 1;
	IIC_Delay();
	SDA = 0;
	IIC_Delay();
	SCL = 0;
	IIC_Delay();
}		


void IIC_Stop(void)					//STOP the IIC, SDA Low-to-high when SCL is high
{
	SCL = 0;
	IIC_Delay();
	SDA = 0;
	IIC_Delay();
	SCL = 1;
	IIC_Delay();
	SDA = 1;
	IIC_Delay();
}

void IIC_Ack(void)              //Send ACK (LOW)
{
	SCL = 0;
	IIC_Delay();
	SDA = 0;
	IIC_Delay();
	SCL = 1;
	IIC_Delay();
	SCL = 0;
	IIC_Delay();
}

void IIC_NAck(void)           //Send No ACK (High)
{
	SCL = 0;
	IIC_Delay();
	SDA = 1;
	IIC_Delay();
	SCL = 1;
	IIC_Delay();
	SCL = 0;
	IIC_Delay();
}
		
u8 IIC_Wait_Ack(void)         //Check ACK, If F0=0, then right, if F0=1, then error
{
	u8 err = 0;
	SDA = 1;
	IIC_Delay();
	SCL = 1;
	IIC_Delay();
	while(SDA)
	{
		err++;
		if (err > 200)
		{
			IIC_Stop();
			return 0;
		}
	}
	SCL = 0;
	IIC_Delay();
	
	return 1;
}

/****************************/
void IIC_Send_Byte(u8 dat)		//write a byte to IIC
{
	u8 i;
	i = 8;
	SCL = 0;
	do
	{
		if(dat & 0x80)	SDA = 1;
		else			SDA	= 0;
		dat <<= 1;
		IIC_Delay();
		SCL = 1;
		IIC_Delay();
		SCL = 0;
		IIC_Delay();
	}
	while(--i);
}

/****************************/
u8 IIC_Read_Byte(unsigned char ack)			//read A byte from IIC
{
	u8 i,dat;
	i = 8;
	SDA = 1;
	SCL = 0;
	do
	{
		SCL = 1;
		IIC_Delay();
		dat <<= 1;
		if(SDA)		dat++;
		SCL  = 0;
		IIC_Delay();
	}
	while(--i);
	
	if (ack == 0)
			IIC_NAck();//·¢ËÍnACK
	else if (ack == 1)
			IIC_Ack(); //·¢ËÍACK   
	
	return(dat);
}

#include "serial_port.h"
#include "config.h"
#include <stdio.h>

unsigned char sending = 0;
unsigned char receiveData;
static char cnt7f = 0;              //7f的计数器

void usart_initial(void)
{
/*
	SCON = 0x50;      //REN=1允许串行接受状态，串口工作模式1     	   
	TMOD|= 0x20;      //定时器工作方式2                       
	PCON|= 0x80;      //波特率提高一倍                                                    	
	TH1 = 0xfa;       //reload value 19200、数据位8、停止位1。效验位无(22.1184M)     				
	TL1 = 0xfa; 
	TR1  = 1;         //开启定时器1  
	IE = 0x0;                                                    
	ES   = 1;         //开串口中断                  
	EA   = 1;         //开总中断 
*/	
	TR1 = 0;
	AUXR &= ~0x01;		//S1 BRT Use Timer1;
	AUXR |=  (1<<6);	//Timer1 set as 1T mode
	TMOD &= ~(1<<6);	//Timer1 set As Timer
	TMOD &= ~0x30;		//Timer1_16bitAutoReload;
	TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
	TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
	ET1 = 0;	//禁止中断
	INT_CLKO &= ~0x02;	//不输出时钟
	TR1  = 1;
	
	SCON = (SCON & 0x3f) | 0x40;	//UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//	PS  = 1;	//高优先级中断
	ES  = 1;	//允许中断
	REN = 1;	//允许接收
	P_SW1 &= 0x3f;
	P_SW1 |= 0x00;		//UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7 (必须使用内部时钟)
//	PCON2 |=  (1<<4);	//内部短路RXD与TXD, 做中继, ENABLE,DISABLE
	
	//EA   = 1;         //开总中断 
	
	sending = 0;
}

void usart_putChar(unsigned char d)
{
	unsigned char retry = 0;
	sending = 1;
	SBUF=d; //将数据写入到串口缓冲
	while(sending)
	{
		retry++;
		if (retry > 200)
		{
			break;
		}
	} //等待发送完毕
}

void usart_putStr(unsigned char *pd)
{
	unsigned char retry = 0;
  for (; *pd != '\0';	pd++)   	//遇到停止符0结束
	{
		SBUF = *pd;
		sending = 1;
		while(sending)
		{
			retry++;
			if (retry > 200)
			{
				break;
			}
		} //等待发送完毕
	}
}

void usart_putString(unsigned char *pd, unsigned char length)
{
	while(length--) //发送字符串，直到遇到0才结束
	{
		usart_putChar(*pd); //发送一个字符
		pd++;  //移动到下一个字符
	}
}

void usart_interrupt(void) interrupt 4
{
	if(RI)        //RI接受中断标志
	{
		RI = 0;		    //清除RI接受中断标志
		receiveData = SBUF;  //SUBF接受/发送缓冲器
		SBUF = receiveData;
		
/* 		if (SBUF == 0x7f)
/* 		{
/* 				if (++cnt7f >= 16)
/* 				{
/* 						ISPPROGRAM();           //调用下载模块(****重要语句****)
/* 				}
/* 		}
/* 		else
/* 		{
/* 				cnt7f = 0;
/* 		}*/
	}
	if(TI)
	{
		TI = 0;
		sending = 0;
	}
}

char putchar(char c)
{
	unsigned char retry = 0;
	sending = 1;
	SBUF=d; //将数据写入到串口缓冲
	while(sending);
	return c;
}
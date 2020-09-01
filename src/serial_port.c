#include "serial_port.h"
#include "config.h"
#include <stdio.h>

unsigned char sending = 0;
unsigned char receiveData;
static char cnt7f = 0;              //7f�ļ�����

void usart_initial(void)
{
/*
	SCON = 0x50;      //REN=1�����н���״̬�����ڹ���ģʽ1     	   
	TMOD|= 0x20;      //��ʱ��������ʽ2                       
	PCON|= 0x80;      //���������һ��                                                    	
	TH1 = 0xfa;       //reload value 19200������λ8��ֹͣλ1��Ч��λ��(22.1184M)     				
	TL1 = 0xfa; 
	TR1  = 1;         //������ʱ��1  
	IE = 0x0;                                                    
	ES   = 1;         //�������ж�                  
	EA   = 1;         //�����ж� 
*/	
	TR1 = 0;
	AUXR &= ~0x01;		//S1 BRT Use Timer1;
	AUXR |=  (1<<6);	//Timer1 set as 1T mode
	TMOD &= ~(1<<6);	//Timer1 set As Timer
	TMOD &= ~0x30;		//Timer1_16bitAutoReload;
	TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
	TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
	ET1 = 0;	//��ֹ�ж�
	INT_CLKO &= ~0x02;	//�����ʱ��
	TR1  = 1;
	
	SCON = (SCON & 0x3f) | 0x40;	//UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//	PS  = 1;	//�����ȼ��ж�
	ES  = 1;	//�����ж�
	REN = 1;	//�������
	P_SW1 &= 0x3f;
	P_SW1 |= 0x00;		//UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7 (����ʹ���ڲ�ʱ��)
//	PCON2 |=  (1<<4);	//�ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE
	
	//EA   = 1;         //�����ж� 
	
	sending = 0;
}

void usart_putChar(unsigned char d)
{
	unsigned char retry = 0;
	sending = 1;
	SBUF=d; //������д�뵽���ڻ���
	while(sending)
	{
		retry++;
		if (retry > 200)
		{
			break;
		}
	} //�ȴ��������
}

void usart_putStr(unsigned char *pd)
{
	unsigned char retry = 0;
  for (; *pd != '\0';	pd++)   	//����ֹͣ��0����
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
		} //�ȴ��������
	}
}

void usart_putString(unsigned char *pd, unsigned char length)
{
	while(length--) //�����ַ�����ֱ������0�Ž���
	{
		usart_putChar(*pd); //����һ���ַ�
		pd++;  //�ƶ�����һ���ַ�
	}
}

void usart_interrupt(void) interrupt 4
{
	if(RI)        //RI�����жϱ�־
	{
		RI = 0;		    //���RI�����жϱ�־
		receiveData = SBUF;  //SUBF����/���ͻ�����
		SBUF = receiveData;
		
/* 		if (SBUF == 0x7f)
/* 		{
/* 				if (++cnt7f >= 16)
/* 				{
/* 						ISPPROGRAM();           //��������ģ��(****��Ҫ���****)
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
	SBUF=d; //������д�뵽���ڻ���
	while(sending);
	return c;
}
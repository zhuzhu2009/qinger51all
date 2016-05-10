#include "delay.h"

void delay_us(unsigned int us)//ÑÓÊ±º¯Êý
{
	unsigned char a = 0;
	while(--us)
	{
		a++;
		a++;
		a++;
	}
}

void delay_ms(unsigned int ms)	//@11.0592MHz
{
	unsigned int a;
	while(ms)
	{
		a=1800;
		while(a--);
		ms--;
	}
}
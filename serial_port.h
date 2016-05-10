#ifndef __SERIAL_PORT_H
#define __SERIAL_PORT_H

#define		Baudrate1			115200L

#define ISPPROGRAM()    ((void (code *)())0xF000)() //ISP下载程序入口地址

void usart_initial(void);
void usart_putChar(unsigned char d);
void usart_putStr(unsigned char *pd);
void usart_putString(unsigned char *pd, unsigned char length);
//void usart_interrupt(void);

#endif
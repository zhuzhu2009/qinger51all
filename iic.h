#ifndef __IIC_H
#define __IIC_H

#include "config.h"
#include "type.h"
#include "delay.h"

#define	SDA P50	//定义SDA  
#define	SCL	P34	//定义SCL  

/* #define	SDA P60	//定义SDA  
/* #define	SCL	P61	//定义SCL  */


//IIC所有操作函数

void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 dat);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号
//
//void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 IIC_Read_One_Byte(u8 daddr,u8 addr);

#endif
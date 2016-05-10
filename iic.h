#ifndef __IIC_H
#define __IIC_H

#include "config.h"
#include "type.h"
#include "delay.h"

#define	SDA P50	//����SDA  
#define	SCL	P34	//����SCL  

/* #define	SDA P60	//����SDA  
/* #define	SCL	P61	//����SCL  */


//IIC���в�������

void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 dat);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�
//
//void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 IIC_Read_One_Byte(u8 daddr,u8 addr);

#endif
#include "key4x4.h"
#include "delay.h"

unsigned char keyscan()//键盘扫描函数，使用行列反转扫描法
{
	unsigned char cord_h,cord_l;//行列值
	KeyData = 0x0f;            //行线输出全为0
	delay_us(2); // very important
	cord_h=KeyData&0x0f;     //读入列线值
	if(cord_h!=0x0f)    //先检测有无按键按下
	{
		delay_us(100);        //去抖
		cord_h=KeyData&0x0f;     //读入列线值
		if(cord_h!=0x0f)
		{
			KeyData=cord_h|0xf0;  //输出当前列线值
			delay_us(2); // very important
			cord_l=KeyData&0xf0;  //读入行线值
			return(cord_h+cord_l);//键盘最后组合码值并返回
		}
	}
	return(0xff);     //否则返回固定值0xff

/* 	unsigned char cord_h,cord_l;//行列值
/* 	KeyData = 0xf0;            //行线输出全为0
/* 	cord_h=KeyData&0xf0;     //读入列线值
/* 	if(cord_h!=0xf0)    //先检测有无按键按下
/* 	{
/* 		delay_us(100);        //去抖
/* 		cord_h=KeyData&0xf0;     //读入列线值
/* 		if(cord_h!=0xf0)
/* 		{
/* 			KeyData=cord_h|0x0f;  //输出当前列线值
/* 			cord_l=KeyData&0x0f;  //读入行线值
/* 			return(cord_h+cord_l);//键盘最后组合码值并返回
/* 		}
/* 	}
/* 	return(0xff);     //否则返回固定值0xff */
	
}


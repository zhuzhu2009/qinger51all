#include "key4x4.h"
#include "delay.h"

unsigned char keyscan()//����ɨ�躯����ʹ�����з�תɨ�跨
{
	unsigned char cord_h,cord_l;//����ֵ
	KeyData = 0x0f;            //�������ȫΪ0
	delay_us(2); // very important
	cord_h=KeyData&0x0f;     //��������ֵ
	if(cord_h!=0x0f)    //�ȼ�����ް�������
	{
		delay_us(100);        //ȥ��
		cord_h=KeyData&0x0f;     //��������ֵ
		if(cord_h!=0x0f)
		{
			KeyData=cord_h|0xf0;  //�����ǰ����ֵ
			delay_us(2); // very important
			cord_l=KeyData&0xf0;  //��������ֵ
			return(cord_h+cord_l);//������������ֵ������
		}
	}
	return(0xff);     //���򷵻ع̶�ֵ0xff

/* 	unsigned char cord_h,cord_l;//����ֵ
/* 	KeyData = 0xf0;            //�������ȫΪ0
/* 	cord_h=KeyData&0xf0;     //��������ֵ
/* 	if(cord_h!=0xf0)    //�ȼ�����ް�������
/* 	{
/* 		delay_us(100);        //ȥ��
/* 		cord_h=KeyData&0xf0;     //��������ֵ
/* 		if(cord_h!=0xf0)
/* 		{
/* 			KeyData=cord_h|0x0f;  //�����ǰ����ֵ
/* 			cord_l=KeyData&0x0f;  //��������ֵ
/* 			return(cord_h+cord_l);//������������ֵ������
/* 		}
/* 	}
/* 	return(0xff);     //���򷵻ع̶�ֵ0xff */
	
}


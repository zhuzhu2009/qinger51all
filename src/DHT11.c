#include "DHT11.h"
#include "delay.h"

unsigned short temperature = 0;
unsigned short humidity = 0;

unsigned char DHT11_ReadValue(void)
{
	unsigned char count, value = 0, i, retry = 0;
	for(i = 8; i > 0; i--)
	{
		//��λ����
		value <<= 1;
		count = 0;
		//ÿһλ����ǰ����һ��50us�ĵ͵�ƽʱ��.�ȴ�50us �͵�ƽ����
		while(dht11 == 0)
		{
			retry++;
			delay_us(1);
			if (retry > 200)
			{
				break;
			}
		}
		if (retry > 200)
		{
			return 0;
		}
		else
		{
			retry = 0;
		}
		//26-28us �ĸߵ�ƽ��ʾ��λ��0,Ϊ70us �ߵ�ƽ���λ1
		delay_us(30);//�����ȷ����30us�ò���5��
		//��ʱ30us �����������Ƿ��Ǹߵ�ƽ
		if(dht11 != 0)
		{
			//���������ʾ��λ��1
			value++;
			//�ȴ�ʣ��(Լ40us)�ĸߵ�ƽ����
			while(dht11 != 0)
			{
				dht11 = 1;	 //����һֱ��һ
				delay_us(1);
				retry++;
				if (retry > 200)
				{
					
					break;
				}
			}
			if (retry > 200)
			{
				return 0;
			}
			else
			{
				retry = 0;
			}
		}
	}
	return (value);
}
//���¶Ⱥ����ʪ�Ⱥ�������һ�ε�����,�����ֽڣ������ɹ���������OK, ���󷵻�ERROR
unsigned char DHT11_ReadTempAndHumi()
{
	unsigned char i = 0, check_value = 0,count = 0, retry = 0;
	unsigned char value_array[5];
	dht11 = 0;    //���������ߴ���18ms ���Ϳ�ʼ�ź�
	delay_ms(21); //�����18 ����
	dht11 = 1;    //�ͷ�������,���ڼ��͵�ƽ��Ӧ���ź�
	//��ʱ20-40us,�ȴ�һ��ʱ�����Ӧ���ź�,Ӧ���ź��Ǵӻ�����������80us
	delay_us(30);	  
	if(dht11 != 0) //���Ӧ���ź�,Ӧ���ź��ǵ͵�ƽ
	{
		return 0;
	}
	else
	{
		//��Ӧ���ź�
		while(dht11 == 0); //�ȴ�Ӧ���źŽ���
		count = 0;
		dht11 = 1;//�ͷ�������
		//Ӧ���źź����һ��80us �ĸߵ�ƽ���ȴ��ߵ�ƽ����
		while(dht11 != 0)
		{
			delay_us(1);
			retry++;
			if (retry > 200)
			{
				break;
			}
		}
		
		if (retry > 200)
		{
			return 0;
		}
		else
		{
			retry = 0;
		}
		
		//����ʪ.�¶�ֵ
		for(i = 0; i < 5; i++)
		{
			value_array[i] = DHT11_ReadValue();
			//���������һ��ֵ��У��ֵ�������ȥ
			if(i != 4)
			{
				//���������ֽ������е�ǰ���ֽ����ݺ͵��ڵ����ֽ����ݱ�ʾ�ɹ�
				check_value += value_array[i];
			}
		}//end for
		//��û�÷�����������ʧ��ʱ����У��
		if(check_value == value_array[4])
		{
			//����ʪ������10 ����������ÿһλ
			humidity = (value_array[0]+(double)value_array[1]/256.0) * 10.0;
			temperature = (value_array[2]+(double)value_array[3]/256.0) * 10.0;
			dht11 = 1;
			return 1; //��ȷ�Ķ���dht11 ���������
		}
		else
		{			//У�����ݳ���
			return 0;
		}
	}
}
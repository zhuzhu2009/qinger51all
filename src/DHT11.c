#include "DHT11.h"
#include "delay.h"

unsigned short temperature = 0;
unsigned short humidity = 0;

unsigned char DHT11_ReadValue(void)
{
	unsigned char count, value = 0, i, retry = 0;
	for(i = 8; i > 0; i--)
	{
		//高位在先
		value <<= 1;
		count = 0;
		//每一位数据前会有一个50us的低电平时间.等待50us 低电平结束
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
		//26-28us 的高电平表示该位是0,为70us 高电平表该位1
		delay_us(30);//仿真后确定的30us用参数5；
		//延时30us 后检测数据线是否还是高电平
		if(dht11 != 0)
		{
			//进入这里表示该位是1
			value++;
			//等待剩余(约40us)的高电平结束
			while(dht11 != 0)
			{
				dht11 = 1;	 //必须一直置一
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
//读温度和相对湿度函数，读一次的数据,共五字节，读出成功函数返回OK, 错误返回ERROR
unsigned char DHT11_ReadTempAndHumi()
{
	unsigned char i = 0, check_value = 0,count = 0, retry = 0;
	unsigned char value_array[5];
	dht11 = 0;    //拉低数据线大于18ms 发送开始信号
	delay_ms(21); //需大于18 毫秒
	dht11 = 1;    //释放数据线,用于检测低电平的应答信号
	//延时20-40us,等待一段时间后检测应答信号,应答信号是从机拉低数据线80us
	delay_us(30);	  
	if(dht11 != 0) //检测应答信号,应答信号是低电平
	{
		return 0;
	}
	else
	{
		//有应答信号
		while(dht11 == 0); //等待应答信号结束
		count = 0;
		dht11 = 1;//释放数据线
		//应答信号后会有一个80us 的高电平，等待高电平结束
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
		
		//读出湿.温度值
		for(i = 0; i < 5; i++)
		{
			value_array[i] = DHT11_ReadValue();
			//读出的最后一个值是校验值不需加上去
			if(i != 4)
			{
				//读出的五字节数据中的前四字节数据和等于第五字节数据表示成功
				check_value += value_array[i];
			}
		}//end for
		//在没用发生函数调用失败时进行校验
		if(check_value == value_array[4])
		{
			//将温湿度扩大10 倍方便分离出每一位
			humidity = (value_array[0]+(double)value_array[1]/256.0) * 10.0;
			temperature = (value_array[2]+(double)value_array[3]/256.0) * 10.0;
			dht11 = 1;
			return 1; //正确的读出dht11 输出的数据
		}
		else
		{			//校验数据出错
			return 0;
		}
	}
}
#include "config.h"
#include "delay.h"
#include "oled.h"
#include "bmp.h"
#include "type.h"
#include "DHT11.h"
#include "PWM.h"
#include "beep.h"
#include "iic.h"
#include "24c02.h"
#include "PCF8563.h"
#include "serial_port.h"
#include "key4x4.h"
#include "NRF24L01.h"
#include "spi.h"

#define	Timer0_Reload	(MAIN_Fosc / 100)		//Timer 0 中断频率, 100次/秒
#define	Timer3_Reload	(MAIN_Fosc / 100)		//Timer 3 中断频率, 100次/秒

u16	Get_ADC10bitResult(u8 channel);
void	Timer0_init(void);
void	Timer3_init(void);
void	PWM_config(void);
void Show_window();
void keyboard_proc();
void task_proc();

u8 KeyCode = 0;
u16 time0_int_cnt = 0;
u8 int0_cnt = 0;
u16 music_pos = 0;
u8 music_first = 1;
u8 time0_int_flag = 0;
u16 task_cnt = 0;

u8 task_flag = 0;
u8 task_num_h = 0;
u8 task_num = 0; 

u16 adc_value = 0;
u32 rp = 0;
u32 rl = 0;

u8 task_run_first = 1;

u8 nrf_send_task = 0;
u8 xdata nrf_buf[32];

u8 rc_out = 0;

void main()
{
	u8 ret = 0;

	P0M1 = 0;	P0M0 = 0;	//设置为准双向口 
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P5M1 = 0; P5M0 = 0;
	P6n_push_pull(1<<2);
	P6n_push_pull(1<<3);
	// adc init
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P1ASF = 0x03;		//P1.0 P1.1做ADC
	ADC_CONTR = 0xE0;	//90T, ADC power on
	// spi init
	SPI_Init();
	// oled init
	OLED_Init();			//初始化OLED  
	OLED_Clear();
	// time0 init
	Timer0_init();
	Timer3_init();
	// ex int0 init
	IE0  = 0;	//外中断0标志位
	EX0 = 1;	//INT0 Enable
	IT0 = 1;		//INT0 下降沿中断	
	// pwm init
	PWM_config();
	// iic init
	IIC_Init();
	// at24c02 init
	AT24CXX_Init();
	// uart init
	usart_initial();
	
	EA = 1;		//打开总中断
	
	delay_ms(500);
	
	ret = AT24CXX_Check();
	if (ret == 0)
	{
		//OLED_ShowString(0,0,"ok");
	}
	else if (ret == 1)
	{
		//OLED_ShowString(0,0,"no");
	}
	else if (ret == 2)
	{
/* 	  //OLED_ShowString(0,0,"okf");
/* 		hour = 15;
/* 		minute = 40;
/* 		second = 30;
/* 		day = 3;
/* 		week = 2;
/* 		month = 5;
/* 		year = 16;
/* 		PCF8563_WriteTime();*/
	}
	
	P62 = 0;
	P63 = 1;
	
	NRF24L01_Init();    //初始化NRF24L01
	
	ret = 0;
	while(NRF24L01_Check() != 1)
	{
		OLED_ShowString(0,0,"24L01 Check Failed!");
		delay_ms(500);
		ret++;
		if (ret >= 2)
		{
			break;
		}
	}
	
	OLED_Clear();
	
	while(1) 
	{		
		Show_window();
		
		task_num_h = task_num;		
		
		keyboard_proc();
		
		task_proc();

		
		/*
			//P62 = ~P62;
			//P63 = ~P63;
			
		adc_value = Get_ADC10bitResult(0);
		//OLED_ShowNum(48,0,adc_value,4,16);
		if (adc_value > 750)
		{
			//OLED_ShowString(48,0,"day!  ");
		}
		else
		{
			//OLED_ShowString(48,0,"night!");
		}
		
		adc_value = Get_ADC10bitResult(1);
		//OLED_ShowNum(48,2,adc_value,4,16);
		
		if (DHT11_ReadTempAndHumi(&temperature, &humidity) == 1)
		{
			OLED_ShowNum(48,4,temperature/10,2,16,' ');
			OLED_ShowNum(72,4,humidity/10,2,16,' ');		
		}
		
		//OLED_ShowNum(48,6,int0_cnt,4,16);
		*/

	}	  
}

u16	Get_ADC10bitResult(u8 channel)	//channel = 0~7
{
	u8 a = 0;
	ADC_RES = 0;
	ADC_RESL = 0;

	ADC_CONTR = (ADC_CONTR & 0xe0) | 0x08 | channel; 	//start the ADC
	a++;a++;a++;a++;

	while((ADC_CONTR & 0x10) == 0)	
	{
		a++;
		if (a > 250)
		{
			break;
		}
	}	//wait for ADC finish
	
	ADC_CONTR &= ~0x10;		//清除ADC结束标志
	return	(((u16)ADC_RES << 2) | (ADC_RESL & 3));
}

void Timer0_init(void)
{
		TR0 = 0;	//停止计数

	#if (Timer0_Reload < 64)	// 如果用户设置值不合适， 则不启动定时器
		#error "Timer0设置的中断过快!"

	#elif ((Timer0_Reload/12) < 65536UL)	// 如果用户设置值不合适， 则不启动定时器
		ET0 = 1;	//允许中断
	//	PT0 = 1;	//高优先级中断
		TMOD &= ~0x03;
		TMOD |= 0;	//工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
	//	TMOD |=  0x04;	//对外计数或分频
		TMOD &= ~0x04;	//定时
	//	INT_CLKO |=  0x01;	//输出时钟
		INT_CLKO &= ~0x01;	//不输出时钟

		#if (Timer0_Reload < 65536UL)
			AUXR |=  0x80;	//1T mode
			TH0 = (u8)((65536UL - Timer0_Reload) / 256);
			TL0 = (u8)((65536UL - Timer0_Reload) % 256);
		#else
			AUXR &= ~0x80;	//12T mode
			TH0 = (u8)((65536UL - Timer0_Reload/12) / 256);
			TL0 = (u8)((65536UL - Timer0_Reload/12) % 256);
		#endif

		//TR0 = 1;	//开始运行

	#else
		#error "Timer0设置的中断过慢!"
	#endif
}

void timer0_int (void) interrupt TIMER0_VECTOR
{
	if (music_first)
	{
		music_first = 0;
		PWM_SourceClk_Duty(PwmClk_1T, music_little_star[music_pos]);	//时钟源: PwmClk_1T,PwmClk_2T, ... PwmClk_16T, PwmClk_T2,   PWM周期: 1~32767
		PWMx_SetPwmWide(PWM7_ID, 0, music_little_star[music_pos]>>1);
		music_pos++;
	}
	else
	{
		time0_int_cnt++;
		if (time0_int_cnt >= music_little_star[music_pos]) //100ms
		{
			time0_int_cnt = 0;
			//P63 = ~P63;
			music_pos++;
			PWM_SourceClk_Duty(PwmClk_1T, music_little_star[music_pos]);
			PWMx_SetPwmWide(PWM7_ID, 0, music_little_star[music_pos]>>1);
			music_pos++;
			if (music_pos >= (sizeof(music_little_star)>>1))
			{
				music_pos = 0;
				music_first = 1;
				TR0 = 0;
				PWMCR &= (~ENPWM);
			}
		}
	}
}

void Timer3_init(void)
{
	Timer3_Stop();	//停止计数

	#if (Timer3_Reload < 64)	// 如果用户设置值不合适， 则不启动定时器
		#error "Timer3设置的中断过快!"

	#elif ((Timer3_Reload/12) < 65536UL)	// 如果用户设置值不合适， 则不启动定时器
		Timer3_AsTimer();
		Timer3_InterruptEnable();	//允许中断
		Timer3_CLKO_Disable();

		#if (Timer3_Reload < 65536UL)
			Timer3_1T();	//1T mode
			T3_Load(65536UL - Timer3_Reload);
		#else
			Timer3_12T();	//12T mode
			T3_Load(65536UL - Timer3_Reload/12);
		#endif

	#else
		#error "Timer3设置的中断过慢!"
	#endif
}
void timer3_int(void) interrupt TIMER3_VECTOR
{
	//PWM_SourceClk_Duty(PwmClk_1T, 256);
	PWMx_SetPwmWide(PWM6_ID, 0, rc_out);
	rc_out+=4;
	if (rc_out >= 40)
	{
		rc_out = 4;
		//P62 = ~P62;
	}
	//P62 = ~P62;
	//usart_putChar('z');
}
void INT0_int (void) interrupt INT0_VECTOR		//进中断时已经清除标志
{
	int0_cnt++;	//中断+1
}

void PWM_config(void)
{
	PWMx_InitDefine		PWMx_InitStructure;					//结构定义
	//beep
	PWMx_InitStructure.PWMx_IO_Select       = PWM7_P17;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM7_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P1n_push_pull(1<<7);										//IO初始化, 上电时为高阻
	
	//TH2 = (u8)((65536UL - MAIN_Fosc / 500000UL) / 256);	//波特率使用Timer2
	//TL2 = (u8)((65536UL - MAIN_Fosc / 500000UL) % 256);
	//AUXR = 0x14;		//Timer2 set As Timer, Timer2 set as 1T mode;
	PWM_SourceClk_Duty(PwmClk_1T, 256);	//时钟源: PwmClk_1T,PwmClk_2T, ... PwmClk_16T, PwmClk_T2,   PWM周期: 1~32767
	PWMx_SetPwmWide(PWM7_ID, 0, 0);
	
	// RC低通滤波器
	PWMx_InitStructure.PWMx_IO_Select       = PWM6_P16;		//PWM输出IO选择. PWM2_P37,PWM2_P27,PWM3_P21,PWM3_P45,PWM4_P22,PWM4_P44,PWM5_P23,PWM5_P42,PWM6_P16,PWM6_P07,PWM7_P17,PWM7_P06
	PWMx_InitStructure.Start_IO_Level       = 0;			//设置PWM输出端口的初始电平, 0或1
	PWMx_InitStructure.PWMx_Interrupt       = DISABLE;		//中断允许,   			ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge_Interrupt  = DISABLE;		//第一个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.SecondEdge_Interrupt = DISABLE;		//第二个翻转中断允许,   ENABLE或DISABLE
	PWMx_InitStructure.FirstEdge            = 64;			//第一个翻转计数, 1~32767
	PWMx_InitStructure.SecondEdge           = 192;			//第二个翻转计数, 1~32767
	PWMx_Configuration(PWM6_ID, &PWMx_InitStructure);		//初始化PWM, 	PWM2_ID,PWM3_ID,PWM4_ID,PWM5_ID,PWM6_ID,PWM7_ID
	P1n_push_pull(1<<6);										//IO初始化, 上电时为高阻
	
	//PWMCR |= ENPWM;		// 使能PWM波形发生器，PWM计数器开始计数
	PWMCR &= ~ECBI;		//禁止PWM计数器归零中断
//	PWMCR |=  ECBI;		//允许PWM计数器归零中断

//	PWMFDCR = ENFD | FLTFLIO | FDIO;	//PWM失效中断控制,  ENFD | FLTFLIO | EFDI | FDCMP | FDIO
	
}

void Show_window()
{
	if (task_num != task_num_h)
	{
		//if (task_num != 16)
		//{
			OLED_Clear();
		//}
	}
	
	if (task_num == 0)
	{
		OLED_ShowChinese(0,0,0);
		OLED_ShowChinese(18,0,1);
		OLED_ShowChinese(36,0,2);
		OLED_ShowChinese(54,0,3);
		OLED_ShowChinese(18,2,4);
		OLED_ShowChinese(36,2,5);
		OLED_ShowString(15,4,"51MCU");
		OLED_ShowString(4,6,"Welcome!");
		OLED_DrawBMP(128-53,0,128,8,BMP1);
	}
	else if (task_num == 1)
	{
		OLED_ShowNum(24+0,0,2000+year,4,16,' ');
		OLED_ShowChar(24+32,0,'-');
		OLED_ShowNum(24+40,0,month,2,16,'0');
		OLED_ShowChar(24+56,0,'-');
		OLED_ShowNum(24+64,0,day,2,16,'0');
		OLED_ShowChinese(40+0,2,6);
		OLED_ShowChinese(40+16,2,7);
		OLED_ShowChinese(40+32,2,week+8);
		OLED_ShowNum(32+0,4,hour,2,16,' ');
		OLED_ShowChar(32+16,4,':');
		OLED_ShowNum(32+24,4,minute,2,16,'0');
		OLED_ShowChar(32+40,4,':');
		OLED_ShowNum(32+48,4,second,2,16,'0');
	}
	else if (task_num == 2)
	{
		OLED_ShowChinese(0,0,15);
		OLED_ShowChinese(16,0,19);
		OLED_ShowChar(32,0,':');
		if (temperature != 0xffff)
		{
			OLED_ShowString(104,0,"  ");
			OLED_ShowNum(88,0,temperature/10,2,16,' ');
		}
		else
		{
			OLED_ShowString(88,0,"err!");
		}
		OLED_ShowChinese(0,2,16);
		OLED_ShowChinese(16,2,17);
		OLED_ShowChinese(32,2,18);
		OLED_ShowChinese(48,2,19);
		OLED_ShowChar(64,2,':');	
		if (temperature != 0xffff)
		{
			OLED_ShowChar(112,2,' ');
			OLED_ShowNum(88,2,humidity/10,2,16,' ');
			OLED_ShowChar(104,2,'%');	
		}
		else
		{
			OLED_ShowString(88,2,"err!");
		}		
		OLED_ShowChinese(0,4,20);
		OLED_ShowChinese(16,4,21);
		OLED_ShowChinese(32,4,22);
		OLED_ShowChinese(48,4,23);
		OLED_ShowChar(64,4,':');
		OLED_ShowNum(88,4,rp,4,16,' ');
		OLED_ShowChinese(0,6,24);
		OLED_ShowChinese(16,6,25);
		OLED_ShowChinese(32,6,26);
		OLED_ShowChar(48,6,':');
		OLED_ShowNum(88,6,rl,4,16,' ');
	}
	else if (task_num == 3)
	{
		OLED_ShowChinese(0,0,27);
		OLED_ShowChinese(16,0,28);
		OLED_ShowChinese(32,0,29);
		OLED_ShowChinese(48,0,30);
		OLED_ShowChar(64,0,':');
		OLED_ShowNum(88,0,int0_cnt,4,16,' ');
	}
	else if (task_num == 4)
	{
		OLED_ShowChinese(40+0,0,31);
		OLED_ShowChinese(40+16,0,32);
		OLED_ShowChinese(40+32,0,33);
	}
	else if (task_num == 5)
	{
		OLED_ShowChinese(32+0,0,34);
		OLED_ShowChinese(32+16,0,35);
		OLED_ShowChinese(32+32,0,36);
		OLED_ShowChinese(32+48,0,37);
		OLED_ShowString(32,2,"send:");
	}
	else if (task_num == 6)
	{
		OLED_ShowChinese(32+0,0,34);
		OLED_ShowChinese(32+16,0,35);
		OLED_ShowChinese(32+32,0,36);
		OLED_ShowChinese(32+48,0,37);
		OLED_ShowString(32,2,"recv:");
	}
	else
	{
		//task_num = 0;
	}
}
void keyboard_proc()
{
	KeyCode = keyscan();
	switch (KeyCode)
	{
		case 0xee: //B1 238
		{
			task_num = 0;
			break;
		}
		case 0xed: //B2 237
		{
			task_num = 1;
			break;
		}
		case 0xeb: //B3 235
		{
			task_num = 2;
			break;
		}
		case 0xe7: //B4 231
		{
			task_num = 3;
			break;
		}
		case 0xde: //B5 222
		{
			task_num = 4;
			break;
		}
		case 0xdd: //B6 221
		{
			task_num = 5;
			break;
		}
		case 0xdb: //B7 219
		{
			task_num = 6;
			break;
		}
		case 0xd7: //B8 215
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '8';
			nrf_buf[2] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0xbe: //B9 190
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '9';
			nrf_buf[2] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0xbd: //B10 189
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '1';
			nrf_buf[2] = '0';
			nrf_buf[3] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0xbb: //B11 187
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '1';
			nrf_buf[2] = '1';
			nrf_buf[3] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0xb7: //B12 183
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '1';
			nrf_buf[2] = '2';
			nrf_buf[3] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0x7e: //B13 126
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '1';
			nrf_buf[2] = '3';
			nrf_buf[3] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0x7d: //B14 125
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '1';
			nrf_buf[2] = '4';
			nrf_buf[3] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0x7b: //B15 123
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '1';
			nrf_buf[2] = '5';
			nrf_buf[3] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			break;
		}
		case 0x77: //B16 119
		{
			nrf_buf[0] = 'B';
			nrf_buf[1] = '1';
			nrf_buf[2] = '6';
			nrf_buf[3] = 0;
			nrf_send_task = (task_num == 5 ? 1 : 0);
			task_num = 16;
			break;
		}		
	}
}
void task_proc()
{
	task_cnt++;
	if (task_num != task_num_h)
	{
		task_cnt = 0;
		task_run_first = 1;
	}
	if (task_num != 0)
	{
		Timer3_Stop();
	}
	if (task_num != 4)
	{
		TR0 = 0;
		PWMx_SetPwmWide(PWM7_ID, 0, 0);
	}
	if (task_num != 0 && task_num != 4)
	{
		PWMCR &= (~ENPWM);
	}
	switch (task_num)
	{
		case 0:
		{
			if (task_run_first == 1)
			{
				PWM_SourceClk_Duty(PwmClk_1T, 128);
				PWMCR |= ENPWM;
				Timer3_Run();	
				task_run_first = 0;	
			}
			if (task_cnt > 20)
			{
				task_cnt = 0;
				P62 = ~P62;
				P63 = ~P63;
			}
			break;
		}
		case 1:
		{
			if (task_cnt > 10)
			{
				task_cnt = 0;
				PCF8563_ReadTime();
			}
			break;
		}
		case 2:
		{
			if (task_cnt > 100)
			{
				task_cnt = 0;
				if (DHT11_ReadTempAndHumi() != 1)
				{
					temperature = 0xfff;
					humidity = 0xffff;
				}
				
				adc_value = Get_ADC10bitResult(0);
				//OLED_ShowNum(48,0,adc_value,4,16);
				rp = (1024 - (u32)adc_value) * 4700ul;
				rp = rp >> 10;
				if (adc_value > 750)
				{
					//OLED_ShowString(48,0,"day!  ");
				}
				else
				{
					//OLED_ShowString(48,0,"night!");
				}
				
				adc_value = Get_ADC10bitResult(1);
				//OLED_ShowNum(48,2,adc_value,4,16);
				rl = (u32)adc_value * 5000ul;
				rl = rl / 1024;
			}

			break;
		}
		case 3:
		{
			break;
		}
		case 4:
		{
			if (task_run_first == 1)
			{
				music_pos = 0;// 每次从头开始放音乐
				music_first = 1;
				PWMCR |= ENPWM;
				TR0 = 1;
				task_run_first = 0;	
			}
			break;
		}
		case 5:
		{
			if (task_run_first == 1)
			{
				TX_Mode();
				delay_ms(100);
				task_run_first = 0;
				nrf_send_task = 0;
			}
			else
			{
				if (nrf_send_task == 1)
				{
					nrf_send_task = 0;
					OLED_ShowString(80,2,"   ");
					OLED_ShowString(80,2,nrf_buf);
					NRF24L01_TxPacket(nrf_buf);
				}
			}
			break;
		}
		case 6:
		{
			if (task_run_first == 1)
			{
				RX_Mode();
				delay_ms(100);
				task_run_first = 0;
			}
			else
			{
				if(NRF24L01_RxPacket(nrf_buf) == 0)//一旦接收到信息,则显示出来.
				{
					OLED_ShowString(80,2,"   ");
					OLED_ShowString(80,2,nrf_buf);
				}
				else
				{
					delay_us(80);
				}
			}
			break;
		}
		case 16:
		{
			if (task_num_h == 1 /* && task_run_first == 1*/)
			{
				hour = 21;
				minute = 20;
				second = 30;
				day = 8;
				week = 0;
				month = 5;
				year = 16;
				PCF8563_WriteTime();
				
				task_run_first = 0;
			}
		}
		default: 
		{
			task_cnt = 0;
			break;
		}
	}
}
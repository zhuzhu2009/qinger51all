#include "beep.h"

void Start_Beep()
{
	PWMCR |= (1 << PWM7_ID);
	//PWMCR |= ENPWM; //��ʹ��
}

void Stop_Beep()
{
	PWMCR &= (~(1 << PWM7_ID));
	//PWMCR &= (~ENPWM); //��ʹ��
}
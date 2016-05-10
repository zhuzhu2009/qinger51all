#include "beep.h"

void Start_Beep()
{
	PWMCR |= (1 << PWM7_ID);
	//PWMCR |= ENPWM; //总使能
}

void Stop_Beep()
{
	PWMCR &= (~(1 << PWM7_ID));
	//PWMCR &= (~ENPWM); //总使能
}
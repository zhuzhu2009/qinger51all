#ifndef __DHT11_H
#define __DHT11_H
#include "config.h"

#define dht11 P51
#define NUMBER 40

extern unsigned short temperature;
extern unsigned short humidity;

unsigned char DHT11_ReadValue(void);
unsigned char DHT11_ReadTempAndHumi(void);

#endif
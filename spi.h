#ifndef __SPI_H
#define __SPI_H	

#include "config.h"
#include "type.h"

#define HW_SPI

#define SPI_CLK		P21
#define SPI_MISO	P22
#define SPI_MOSI	P23

void SPI_Init(void);
u8 SPI_RW(u8 wd);

#endif
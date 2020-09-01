#include "spi.h"

#define HW_SPI

#define SPI_CLK		P21
#define SPI_MISO	P22
#define SPI_MOSI	P23

void SPI_Init(void)
{
	P2n_push_pull(1<<1);
	//P2n_push_pull(1<<2);
	P2n_push_pull(1<<3);

#ifdef HW_SPI
	SPCTL |=  (1 << 7);	//disable SS, conform Master or Slave by SPI_Mode
	SPCTL |=  (1 << 6);	//SPI enable
	SPCTL &= ~(1 << 5);	//MSB first
	SPCTL |=  (1 << 4);	//master
	SPCTL &= ~(1 << 3);	//SCLK Idle Low, High Active.
	SPCTL &= ~(1 << 2);	//sample at the first  edge
	SPCTL = (SPCTL & ~3) | 3;	//set speed, 0: 4T, 1: 16T,  2: 64T,  3: 128T
	AUXR1 = (AUXR1 & ~(3<<2)) | (1<<2);		//IO口切换. 0: P1.2 P1.3 P1.4 P1.5, 1: P2.4 P2.3 P2.2 P2.1, 2: P5.4 P4.0 P4.1 P4.3

	SPI_CLK = 0;	// set clock to low initial state
	SPI_MOSI = 1;
	SPSTAT = SPIF + WCOL;	//清0 SPIF和WCOL标志

#else

#endif
}

u8 SPI_RW(u8 wd)
{
	//u8 rd = 0;
	
#ifdef HW_SPI
	u8 retry = 0;
	SPDAT = wd;
	while((SPSTAT & SPIF) == 0)
	{
		retry++;
		if (retry > 200) 
		{
			break;
		}
	}
	SPSTAT = SPIF + WCOL;	//清0 SPIF和WCOL标志
	
	return SPDAT;
#else
	u8 i = 0;
	for(i=0;i<8;i++)
	{			  
		SPI_CLK = 0;
		SPI_MOSI = wd&0x80;
		wd <<= 1;  
		SPI_CLK = 1;
		wd |= SPI_MISO;
	}	
#endif
	
	return wd;
}
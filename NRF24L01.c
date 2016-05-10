#include "delay.h"
#include "NRF24L01.h"

//#include "serial_port.h"

const unsigned char TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const unsigned char RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

//��ʼ��24L01��IO��
void NRF24L01_Init(void)
{
  P6n_push_pull(1<<6);
	P6n_push_pull(1<<7);		 		  
}

/* unsigned char SPI_RW(unsigned char byte)
/* {
/* 	unsigned char bit_ctr;
/* 	for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
/* 	{
/* 		MOSI = (byte & 0x80)>>7; // output 'byte', MSB to MOSI
/* 		byte = (byte << 1); // shift next bit into MSB..
/* 		SCK = 1; // Set SCK high..
/* 		byte |= MISO; // capture current MISO bit
/* 		SCK = 0; // ..then set SCK low again
/* 	}
/* 	return(byte); // return read byte
/* }*/

unsigned char SPI_Read_reg(unsigned char reg)
{
	unsigned char reg_val;
	CSN = 0; // CSN low, initialize SPI communication...
	SPI_RW(reg); // Select register to read from..
	reg_val = SPI_RW(0xff); // ..then read registervalue
	CSN = 1; // CSN high, terminate SPI communication
	return(reg_val); // return register value
}

// ��Ĵ���regдһ���ֽڣ�ͬʱ����״̬�ֽ�
unsigned char SPI_Write_Reg(unsigned char reg, unsigned char value)
{
	unsigned char status;
	CSN = 0; // CSN low, init SPI transaction
	status = SPI_RW(reg); // select register
	SPI_RW(value); // ..and write value to it..
	CSN = 1; // CSN high again
	return(status); // return nRF24L01 status byte
}

// ����bytes�ֽڵ�����
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	unsigned char status,byte_ctr;
	CSN = 0;                                      // Set CSN low, init SPI tranaction
	status = SPI_RW(reg);                         // Select register to write to and read status byte
	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
		pBuf[byte_ctr] = SPI_RW(0xff);               // ������Ĳ��ѼĴ���ֵ��������
	CSN = 1;
	return(status);                               // return nRF24L01 status byte
}

unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	unsigned char status,byte_ctr;
	CSN = 0;
	status = SPI_RW(reg);
	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)  //
		SPI_RW(*pBuf++);
	CSN = 1;                                     // Set CSN high again
	return(status); //
}

unsigned char NRF24L01_Check(void)
{
	unsigned char buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	unsigned char i;
	CE = 0;
	SPI_Write_Buf(WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	SPI_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ 
	for(i=0;i<5;i++)
	{	
		if(buf[i]!=0XA5)
			break;
	}	 							   
	if(i!=5)
		return 0;//���24L01����	
	return 1;		 //��⵽24L01
}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
unsigned char NRF24L01_TxPacket(unsigned char *txbuf)
{
	unsigned char sta;
	CE=0;
  	SPI_Write_Buf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
 	CE=1;                                          //��������	   
	while(IRQ!=0);                                 //�ȴ��������
	//delay_us(10);
	sta=SPI_Read_Reg(STATUS);                      //��ȡ״̬�Ĵ�����ֵ	   
	SPI_Write_Reg(WRITE_REG+STATUS,sta);           //���TX_DS��MAX_RT�жϱ�־
	if(sta&MAX_TX)                                 //�ﵽ����ط�����
	{
		SPI_Write_Reg(FLUSH_TX,0xff);              //���TX FIFO�Ĵ��� 
		//LCD_ShowString(8, 80, colors[3], colors[1], "      MAX_TX");
		//LCD_ShowString(8, 80, colors[3], colors[1], "            ");
		return MAX_TX; 
	}
	if(sta&TX_OK)                                  //�������
	{
		//LCD_ShowString(8, 80, colors[3], colors[1], "      TX_OK");

		return TX_OK;
	}
	//LCD_ShowString(8, 80, colors[3], colors[1], "      0xff");
	delay_us(100);
	//LCD_ShowString(8, 80, colors[3], colors[1], "            ");
	return 0xff;                                   //����ԭ����ʧ��
}
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
unsigned char NRF24L01_RxPacket(unsigned char *rxbuf)
{
	unsigned char sta;		    							   
	sta=SPI_Read_Reg(STATUS);                      //��ȡ״̬�Ĵ�����ֵ    	 
	SPI_Write_Reg(WRITE_REG+STATUS,sta);           //���TX_DS��MAX_RT�жϱ�־
	if(sta&RX_OK)                                  //���յ�����
	{
		SPI_Read_Buf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
		SPI_Write_Reg(FLUSH_RX,0xff);              //���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;                                      //û�յ��κ�����
}				

void RX_Mode(void)
{
	CE=0;
//	CSN = 1;
//	SCK = 0;
//	CE = 0;
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
	SPI_Write_Reg(WRITE_REG + EN_AA, 0x01);                       // Enable Auto.Ack:Pipe0
	SPI_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);                   // Enable Pipe0
	SPI_Write_Reg(WRITE_REG + SETUP_AW, 0x02);                    // Setup address width=5 bytes
	SPI_Write_Reg(WRITE_REG + RF_CH, 40);                         // Select RF channel 40
	SPI_Write_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);
	SPI_Write_Reg(WRITE_REG + RF_SETUP, 0x07);
	SPI_Write_Reg(WRITE_REG + CONFIG, 0x0f);                      // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
	CE = 1;                                                    // Set CE pin high to enable RX device
}

void TX_Mode(void)
{
	CE=0;
	CSN = 1;
	//SCK = 0;
	CE = 0;
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
	SPI_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);                            // Enable Pipe0
	SPI_Write_Reg(WRITE_REG + EN_AA, 0x01);                                // Enable Auto.Ack:Pipe0
	SPI_Write_Reg(WRITE_REG + SETUP_AW, 0x02);                             // Setup address width=5 bytes
	SPI_Write_Reg(WRITE_REG + SETUP_RETR, 0x1a);                           // 500us + 86us, 10 retrans...
	SPI_Write_Reg(WRITE_REG + RF_CH, 40);                                  // Select RF channel 40
	SPI_Write_Reg(WRITE_REG + RF_SETUP, 0x07);                             // TX_PWR:0dBm, Datarate:2Mbps,LNA:HCURR
	SPI_Write_Reg(WRITE_REG + CONFIG, 0x0e);                               // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
	CE=1;
}

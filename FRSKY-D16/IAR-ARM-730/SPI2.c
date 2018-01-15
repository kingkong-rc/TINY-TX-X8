#include "include.h"

void SPI2_Init(void)
{
  	//GPIO_INIT ： MOSI  SCK  
	GPIOG -> CR1 |= (1<<6)|(1<<5) ; 
	GPIOG -> CR2 |= (1<<6)|(1<<5) ; 
	GPIOG -> DDR |= (1<<6)|(1<<5) ; 
	GPIOG -> ODR &=~((1<<6)|(1<<5));
	
	// MISO
	GPIOG -> CR1 &= ~(1<<7) ; 
	GPIOG -> CR2 &= ~(1<<7) ; 
	GPIOG -> DDR &= ~(1<<7) ; 
	
	CLK -> PCKENR3 |= (1<<2) ; 
	
	//SPI2_Init
	SPI2 -> CR1 &= ~(1<<6) ; 
	SPI2 -> CR1 |= (1<<5)|(1<<3)|(1<<2) ; 					//初始化 MSB first  使能 SPI f = 12M/64 = 187.5Khz(初始化的时候先设置为低速模式) 上升沿采样
	SPI2 -> CR2 |= (1<<1)|(1<<0) ; 
	
	//使能SPI
	SPI2 -> CR1 |= (1<<6) ; 
}


void SPI2_wr_Byte(uint8_t Wdata)
{
  	while(!(SPI2 -> SR & (1<<1)));						//写入发送缓存数据前，先等待发送缓存空。
	SPI2 -> DR = Wdata ; 
	while(!(SPI2 -> SR & (1<<0)));						//等待SPI2发送完成
	Wdata = SPI2 -> DR ; 
	GPIOG -> ODR |= (1<<6);
}


uint8_t SPI2_rd_Byte(void)
{
  	uint8_t Rdata = 0 ; 
	
	SPI2 -> DR = 0xff ; 
	while(!(SPI2 -> SR & (1<<0))) ; 					//等待 SPI2 接收缓存 非空
	Rdata = SPI2 -> DR ; 
	
	return Rdata ; 
}
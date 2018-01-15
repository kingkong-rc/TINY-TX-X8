#ifndef _SPI2_H_
#define _SPI2_H_

void SPI2_Init(void);

void SPI2_wr_Byte(uint8_t Wdata);
uint8_t SPI2_rd_Byte(void);

//void SPI2_TX_Data(uint8_t* tx , uint8_t tx_length);
//void SPI2_RX_Data(uint8_t* rx , uint8_t rx_length);

//void DMA1_CH0_RX_isr(void);
//void DMA1_CH3_TX_isr(void);

#endif

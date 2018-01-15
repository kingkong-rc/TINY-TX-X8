#ifndef __ee_H_
#define __ee_H_

//极值校准标志位
#define IS_EVER_MIDCALIBRATION_FLG	0x55

//校准数据存放位置
#define IS_EVER_MIDCALIBRATION_ADDR 	0x00

#define RUD_MAXVALUE_ADDR	        0x02
#define RUD_MIDVALUE_ADDR           	0x04
#define RUD_MINVALUE_ADDR               0x06

#define THR_MAXVALUE_ADDR	        0x08
#define THR_MIDVALUE_ADDR           	0x0A
#define THR_MINVALUE_ADDR               0x0C

#define ELE_MAXVALUE_ADDR	        0x0E
#define ELE_MIDVALUE_ADDR           	0x10
#define ELE_MINVALUE_ADDR               0x12

#define AIL_MAXVALUE_ADDR	        0x14
#define AIL_MIDVALUE_ADDR           	0x16
#define AIL_MINVALUE_ADDR               0x18

#define AUX1VALUE_ADDR			0x1A			
#define AUX2VALUE_ADDR			0x1C
#define AUX3VALUE_ADDR			0x1E			
#define AUX4VALUE_ADDR			0x20

#define RUD_OFFSET_ADDR			0x22
#define THR_OFFSET_ADDR			0x23
#define ELE_OFFSET_ADDR			0x24
#define AIL_OFFSET_ADDR			0x25

#define TransmitterProtocolVersion_ADDR	0x26
#define TIM2ErrorCnt_ADDR	        0x27

#define IS_EVER_INITEEPROM_ADDR		0x30
#define IS_EVER_INITEEPROM_FLG		0x1234

bool Write_EE_Byte(unsigned char WDat ,unsigned short WAddr);
unsigned char Read_EE_Byte(unsigned short RAddr);
uint16_t Read_EE_Word(unsigned short RAddr);
void Init_EEPROM_Dat(void);

#endif

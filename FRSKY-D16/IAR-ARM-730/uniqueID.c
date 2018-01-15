#include "include.h"

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//由单片机的96bit唯一ID , 换算出 16Bit的 唯一ID(注意 : STM8L052R8 PDF 无说明)
//参考 STM8L151K6 PDF
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
uint16_t GetUniqueID(void)
{
  	uint8_t MCU_UniqueID[12] ; 
	uint16_t ID = 0 ; 
	
	MCU_UniqueID[0]  = *(uint16_t *)(0x4926) ; 
	MCU_UniqueID[1]  = *(uint16_t *)(0x4927) ; 
	MCU_UniqueID[2]  = *(uint16_t *)(0x4928) ; 
	MCU_UniqueID[3]  = *(uint16_t *)(0x4929) ; 
	
	MCU_UniqueID[4]  = *(uint16_t *)(0x492A) ; 
	MCU_UniqueID[5]  = *(uint16_t *)(0x492B) ; 
	MCU_UniqueID[6]  = *(uint16_t *)(0x492C) ; 
	MCU_UniqueID[7]  = *(uint16_t *)(0x492D) ; 
	
	MCU_UniqueID[8]  = *(uint16_t *)(0x492E) ; 
	MCU_UniqueID[9]  = *(uint16_t *)(0x492F) ; 
	MCU_UniqueID[10] = *(uint16_t *)(0x4930) ; 
	MCU_UniqueID[11] = *(uint16_t *)(0x4931) ; 
	
	//SFHSS 只需要 16Bit 的 ID 号 (0 + 2 + 4 + 6 + 8 + 10)    (1+ 3 + 5 + 7 + 9 + 11)
 	ID = MCU_UniqueID[0] + MCU_UniqueID[2] + MCU_UniqueID[4] + MCU_UniqueID[6] + MCU_UniqueID[8] + MCU_UniqueID[10] ;	
	ID = (ID<<8) + MCU_UniqueID[1] + MCU_UniqueID[3] + MCU_UniqueID[5] + MCU_UniqueID[7] + MCU_UniqueID[9] + MCU_UniqueID[11] ;	
	
	return ID ; 
}
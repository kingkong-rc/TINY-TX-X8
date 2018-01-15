/*******************************************************************************
--------------------------------------------------------------------------------
MCU : STM8LXXX
--------------------------------------------------------------------------------
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
特别注意 : 
RWW : Read while write (只有High-density 才有) ，但是注意每次只能操作一个Byte
所以一次读写超过1Byte时需要分段进行。否则可能导致 MCU 短暂无法响应(包括中断)，直到EEPROM操作完成。
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
--------------------------------------------------------------------------------

*******************************************************************************/

#include "include.h"

////////////////////////////////////////////////////////////////////////////////
//                         向输入地址处写入单字节数据                         //
//	             地址: unsigned short  数据：unsigned char                //
////////////////////////////////////////////////////////////////////////////////
bool Write_EE_Byte(unsigned char WDat ,unsigned short WAddr)
{
  	//写入秘钥:解锁EE写操作
	FLASH -> DUKR = 0xAE ;
	FLASH -> DUKR = 0x56 ; 

	//防止写地址越位
	if(WAddr > 0xFF) WAddr = 0xFF ; 
	
	//将要写入的数据  导入对应地址内
	*(unsigned char *)(WAddr + 0x1000) = WDat ; 
	
	//等待 标志位置位 
	unsigned char temp_flg = 0 ;
	unsigned int  Waite_Time = 0xFFFF ;
	while((temp_flg == 0) && (Waite_Time != 0))
	{
	  	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  	//此处特别注意 : 只能读一次IAPSR寄存器，否则可能出现错误
	  	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  	temp_flg = ((FLASH -> IAPSR) & ((1<<2)|(1<<0))) ;
        	--Waite_Time ; 
	}
	
	//EEPROM写保护使能
	FLASH -> IAPSR &= ~(1<<3);
	
	//回读编程地址数据 : 如果和写入数据一致，说明写操作成功   如果不一致，写操作失败
	unsigned char temp = *(unsigned char *)(WAddr + 0x1000) ; 
	
	if(temp == WDat)
	{
		return true ; 
	}
	else
	{
	 	return false ;
	}
}

////////////////////////////////////////////////////////////////////////////////
//                             读取输入地址单字节数据                         //
////////////////////////////////////////////////////////////////////////////////
unsigned char Read_EE_Byte(unsigned short RAddr)
{
	if(RAddr > 0xFF) RAddr = 0xFF ;
	  
	unsigned char RDat = *(unsigned char *)(RAddr + 0x1000) ; 
	return RDat ; 
}

////////////////////////////////////////////////////////////////////////////////
//                             读取输入地址双字节数据                         //
////////////////////////////////////////////////////////////////////////////////
uint16_t Read_EE_Word(unsigned short RAddr)
{
	if(RAddr > 0xFF) RAddr = 0xFF ;  
	  
	uint16_t RDat = *(uint16_t *)(RAddr + 0x1000) ; 
	return RDat ; 
}

////////////////////////////////////////////////////////////////////////////////
//                            初始化读取EEPROM数据                            //
////////////////////////////////////////////////////////////////////////////////
void Init_EEPROM_Dat(void)
{
  	uint16_t Temp = 0 ; 
	
	//======================================================================
	//查看设备是否第一次上电(初始化所有EEPROM参数 ，直接进入中位校准)
	//======================================================================
	Temp = Read_EE_Word(IS_EVER_INITEEPROM_ADDR) ; 
	if(Temp != IS_EVER_INITEEPROM_FLG)
	{
		//==============================================================
	  	//更新第一上电标志位(0x1234)
		//==============================================================
		Write_EE_Byte(IS_EVER_INITEEPROM_FLG >> 8     , IS_EVER_INITEEPROM_ADDR);
		Write_EE_Byte(IS_EVER_INITEEPROM_FLG & 0xFF   , IS_EVER_INITEEPROM_ADDR + 1);
		
		//==============================================================
		//5-6通道，默认初始化为最低档位
		//==============================================================
		Sampling_Data[AUX1] = ADC_AUX_LOW ;  
		Write_EE_Byte(Sampling_Data[AUX1] >> 8    , AUX1VALUE_ADDR);
		Write_EE_Byte(Sampling_Data[AUX1] & 0xFF  , AUX1VALUE_ADDR + 1);
		
		Sampling_Data[AUX2] = ADC_AUX_LOW ;  
		Write_EE_Byte(Sampling_Data[AUX2] >> 8    , AUX2VALUE_ADDR);
		Write_EE_Byte(Sampling_Data[AUX2] & 0xFF  , AUX2VALUE_ADDR + 1);
		
		//==============================================================
		// 初始化偏置(默认初始化为 50(无偏置  0/100 最小 最大偏置))
		//==============================================================
		Sampling_Offset[RUDDER]   = 50 ; 
		Write_EE_Byte(50   , RUD_OFFSET_ADDR);
		
		Sampling_Offset[THROTTLE] = 50 ; 
		Write_EE_Byte(50   , THR_OFFSET_ADDR);
		
		Sampling_Offset[ELEVATOR] = 50 ; 
		Write_EE_Byte(50   , ELE_OFFSET_ADDR);
		
		Sampling_Offset[AILERON]  = 50 ; 
		Write_EE_Byte(50   , AIL_OFFSET_ADDR);
		
		//==============================================================
		// 初始化TIM2死机次数 : 0 次
		//==============================================================
		TIM2_ErrorCnt = 0 ; 
		Write_EE_Byte(TIM2_ErrorCnt , TIM2ErrorCnt_ADDR);
		
		//==============================================================
		//标记当前中位校准无效 , 需要重新进行中位校准
		//==============================================================
		FTDebug_err_flg = true ; 
	}
	
	//======================================================================
	else
	{
  		//======================================================================
		//	档位存储 : AUX1(1200 / 1800)   AUX2(1200 / 1500 / 1800)
  		//======================================================================
  		Temp = Read_EE_Word(AUX1VALUE_ADDR) ; 
		if(Temp > ADC_AUX_HIGH) Temp = ADC_AUX_HIGH ; 
		if(Temp >= ADC_AUX_HIGH)     			{Sampling_Data[AUX1] = ADC_AUX_HIGH ; }
		else 						{Sampling_Data[AUX1] = ADC_AUX_LOW  ; }
	
		Temp = Read_EE_Word(AUX2VALUE_ADDR) ; 
		if(Temp > ADC_AUX_HIGH) Temp = ADC_AUX_HIGH ; 
		if(Temp >= ADC_AUX_HIGH)     			{Sampling_Data[AUX2] = ADC_AUX_HIGH ; }
		else if(Temp >= ADC_AUX_MID) 			{Sampling_Data[AUX2] = ADC_AUX_MID  ; }
		else 						{Sampling_Data[AUX2] = ADC_AUX_LOW  ; }
		
		//==============================================================
		//			微调偏置值
		//==============================================================
		Temp = Read_EE_Byte(RUD_OFFSET_ADDR) ;
		if(Temp > 100) Temp = 50 ; 
		Sampling_Offset[RUDDER] = Temp ; 
		
		Temp = Read_EE_Byte(THR_OFFSET_ADDR) ;
		if(Temp > 100) Temp = 50 ; 
		Sampling_Offset[THROTTLE] = Temp ; 
		
		Temp = Read_EE_Byte(ELE_OFFSET_ADDR) ;
		if(Temp > 100) Temp = 50 ; 
		Sampling_Offset[ELEVATOR] = Temp ; 
		
		Temp = Read_EE_Byte(AIL_OFFSET_ADDR) ;
		if(Temp > 100) Temp = 50 ; 
		Sampling_Offset[AILERON] = Temp ; 
	  
		//==============================================================
		//		  TIM2 死机次数
		//==============================================================
		TIM2_ErrorCnt = Read_EE_Byte(TIM2ErrorCnt_ADDR) ;

		//==============================================================
		//是否进行工厂校准:
		//(1)是 : 读取四摇杆的最大、最小、中值、偏置
		//(2)否 : 强制进行中位校准
		//==============================================================
		//工厂校准标志位
		Temp = Read_EE_Byte(IS_EVER_MIDCALIBRATION_ADDR) ; 
		if(Temp == IS_EVER_MIDCALIBRATION_FLG) 
		{
		  	//======================================================
		  	//     如果校准过中位值，清零。接下来判断中位值有效性。
		  	//======================================================
	  		FTDebug_err_flg = false ; 
		
			//======================================================
			//           判断存储的各中位校准数据是否有效
			//======================================================
  			Temp = Read_EE_Word(RUD_MAXVALUE_ADDR) ; 		// RUD 最大值
			if(Temp < MAXValue_Min)					
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[RUDDER][MAXDAT] = Temp ;
			}
	
			
  			Temp = Read_EE_Word(RUD_MIDVALUE_ADDR) ; 
			if((Temp < AD_MidValue_Min) || (Temp > AD_MidValue_Max))//RUD 中值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[RUDDER][MIDDAT] = Temp ;
			}
			
  			Temp = Read_EE_Word(RUD_MINVALUE_ADDR) ; 
			if(Temp > MINValue_Max)					//RUD最小值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[RUDDER][MINDAT] = Temp ;
			}
			
			
	
  			Temp = Read_EE_Word(THR_MAXVALUE_ADDR) ; 
			if(Temp < MAXValue_Min)					//THR最大值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[THROTTLE][MAXDAT] = Temp ;
			}
	
			//THR 没有中位校准
			
  			Temp = Read_EE_Word(THR_MINVALUE_ADDR) ; 
			if(Temp > MINValue_Max)					//THR最小值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[THROTTLE][MINDAT] = Temp ;
			}
	
			
			
  			Temp = Read_EE_Word(ELE_MAXVALUE_ADDR) ; 
			if(Temp < MAXValue_Min)					//ELE最大值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[ELEVATOR][MAXDAT] = Temp ;
			}
	
			
  			Temp = Read_EE_Word(ELE_MIDVALUE_ADDR) ; 
			if((Temp < AD_MidValue_Min) || (Temp > AD_MidValue_Max))//ELE中值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[ELEVATOR][MIDDAT] = Temp ;
			}
			
  			Temp = Read_EE_Word(ELE_MINVALUE_ADDR) ; 
			if(Temp > MINValue_Max)					//ELE最小值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[ELEVATOR][MINDAT] = Temp ;
			}
	
  			Temp = Read_EE_Word(AIL_MAXVALUE_ADDR) ; 
			if(Temp < MAXValue_Min)					//AIL最大值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[AILERON][MAXDAT] = Temp ;
			}
	
			
			
  			Temp = Read_EE_Word(AIL_MIDVALUE_ADDR) ; 
			if((Temp < AD_MidValue_Min) || (Temp > AD_MidValue_Max))//AIL中值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[AILERON][MIDDAT] = Temp ;
			}
			
  			Temp = Read_EE_Word(AIL_MINVALUE_ADDR) ; 
			if(Temp > MINValue_Max)					//AIL最小值
			{
		  		FTDebug_err_flg = true ;
			}
			else
			{
				Sampling_MaxMinData[AILERON][MINDAT] = Temp ;
			}
		}
		else 
		{
			FTDebug_err_flg = true ; 
		}
	}
}


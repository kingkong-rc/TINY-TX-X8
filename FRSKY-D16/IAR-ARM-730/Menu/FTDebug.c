/******************************************************************************
--------------------------------------------------------------------------------
进入极值标定方法 : 按住 CH6(下) + 右五维按键(Enter) 上电，进入极值标定
--------------------------------------------------------------------------------
工厂校准 : 
(1)标定 Rud 、 AIL 、 ELE 摇杆的中位值(取10次有效值，去掉最大最小值，再求平均)
(2)标定 Rud 、 THR  、AIL 、 ELE 摇杆的最大最小值(取10次有效值，去掉最大最小值，再求平均)
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
*******************************************************************************/
#include "include.h"

bool   FTDebug_err_flg = false ; 						//中位校准故障标志位(中位值数据无效)
static bool     MAXValueDebug_RightOrLeftflg = false ; 				//极值校准左摇杆还是右摇杆标志位
static uint8_t  SamplingCounts = 0 ; 						//采样次数
static uint8_t  OneSamplingDelayTime = 0 ; 					//每次采样延时时间(考虑到采样数据缓存是每10mS更新一次，所以中位标定的数据缓存也每10mS更新一次)
static uint16_t SamplingBuffer[4][10] ;  					//采样数据缓存
static uint16_t Sampling_OK_DelayCNT = 0 ; 					//采样完成延时跳转计数
static uint16_t DisLEDCnts = 0 ; 
static uint16_t SamplingMaxMinValue_StabilizeCnts = 0 ; 			//采样极值稳定计数

//==============================================================================
//获取10次三方摇杆中位值(松开摇杆时的数据)
//去掉一个最大值，一个最小值，其余8个求平均
//==============================================================================
static void FT_GetHSKMidValue(void)
{
  	uint16_t MaxValue_Temp = 0 ; 						//标记数组中最大值
	uint16_t MinValue_Temp = 0 ; 						//标记数组中最小值
	uint32_t SumValue_Temp = 0 ; 						//数组累加和
	uint16_t Temp = 0 ; 
	uint8_t  i = 0 ; 

	if(SamplingCounts < 10) 
	{
	  	if(OneSamplingDelayTime < 5)
		{
			++OneSamplingDelayTime ; 
		}
		else
		{
			SamplingBuffer[RUDDER][SamplingCounts]   = Sampling_Data[RUDDER]; 
			SamplingBuffer[ELEVATOR][SamplingCounts] = Sampling_Data[ELEVATOR];
			SamplingBuffer[AILERON][SamplingCounts]  = Sampling_Data[AILERON];
			OneSamplingDelayTime = 0 ; 
			++SamplingCounts ; 	
		}
	}
	else
	{
	  	//==============================================================
	  	//初始化 LED 状态
	  	//==============================================================
		LED_State_Shake = LED_CH5_L | LED_CH6_H | LED_CH6_L ;
		LED_State_ON = 0x00 ; 
	  
	  	//初始化清零中位校准故障标志位
		FTDebug_err_flg = false ; 
		
		//==============================================================
	  	//每方位去掉最大、最小值。求平均得到中位值
	  	//==============================================================
	  	//初始化极值
	  	MaxValue_Temp = Input_Min ; 
		MinValue_Temp = Input_Max ; 
		SumValue_Temp = 0 ; 
	  	for(i = 0; i < 10 ;i++)
		{
			if(MaxValue_Temp < SamplingBuffer[RUDDER][i]) MaxValue_Temp = SamplingBuffer[RUDDER][i] ; 
			if(MinValue_Temp > SamplingBuffer[RUDDER][i]) MinValue_Temp = SamplingBuffer[RUDDER][i] ; 
			SumValue_Temp += SamplingBuffer[RUDDER][i] ; 
		}
		Temp = (uint16_t)((SumValue_Temp - MaxValue_Temp - MinValue_Temp) / 8) ; 
		if((Temp < AD_MidValue_Min) || (Temp > AD_MidValue_Max)) 		 //中位值校准有效性判断
		{
			FTDebug_err_flg = true ; 
			LED_State_Shake |= LED_CH5_L ; LED_State_ON &= ~LED_CH5_L ;	 // RUDDER 中位值 校准故障提示  CH5_L LED闪烁
		}
		else
		{
		  	LED_State_ON |= LED_CH5_L ; LED_State_Shake &= ~LED_CH5_L ;	 // RUDDER 中位值 校准正常提示
			Sampling_MaxMinData[RUDDER][MIDDAT] = Temp ;
		}
		
	  	//初始化极值
	  	MaxValue_Temp = Input_Min ; 
		MinValue_Temp = Input_Max ; 
		SumValue_Temp = 0 ; 
	  	for(i = 0; i < 10 ;i++)
		{
			if(MaxValue_Temp < SamplingBuffer[ELEVATOR][i]) MaxValue_Temp = SamplingBuffer[ELEVATOR][i] ; 
			if(MinValue_Temp > SamplingBuffer[ELEVATOR][i]) MinValue_Temp = SamplingBuffer[ELEVATOR][i] ; 
			SumValue_Temp += SamplingBuffer[ELEVATOR][i] ; 
		}
		Temp = (uint16_t)((SumValue_Temp - MaxValue_Temp - MinValue_Temp) / 8) ;  
		//中位值校准有效性判断
		if((Temp < AD_MidValue_Min) || (Temp > AD_MidValue_Max)) 
		{
			FTDebug_err_flg = true ; 
			LED_State_Shake |= LED_CH6_H ; LED_State_ON &= ~LED_CH6_H ;	 // ELEVATOR 中位值 校准故障提示
		}
		else
		{
		  	LED_State_ON |= LED_CH6_H ; LED_State_Shake &= ~LED_CH6_H ;	 // ELEVATOR 中位值 校准正常提示
			Sampling_MaxMinData[ELEVATOR][MIDDAT] = Temp ; 
		}
		
	  	//初始化极值
	  	MaxValue_Temp = Input_Min ; 
		MinValue_Temp = Input_Max ; 
		SumValue_Temp = 0 ; 
	  	for(i = 0; i < 10 ;i++)
		{
			if(MaxValue_Temp < SamplingBuffer[AILERON][i]) MaxValue_Temp = SamplingBuffer[AILERON][i] ; 
			if(MinValue_Temp > SamplingBuffer[AILERON][i]) MinValue_Temp = SamplingBuffer[AILERON][i] ; 
			SumValue_Temp += SamplingBuffer[AILERON][i] ; 
		}
		Temp = (uint16_t)((SumValue_Temp - MaxValue_Temp - MinValue_Temp) / 8) ; 
		//中位值校准有效性判断
		if((Temp < AD_MidValue_Min) || (Temp > AD_MidValue_Max)) 
		{
			FTDebug_err_flg = true ; 
			LED_State_Shake |= LED_CH6_L ; LED_State_ON &= ~LED_CH6_L ;	 // AILERON 中位值 校准故障提示
		}
		else
		{
		  	LED_State_ON |= LED_CH6_L ; LED_State_Shake &= ~LED_CH6_L ;	 // AILERON 中位值 校准正常提示
			Sampling_MaxMinData[AILERON][MIDDAT]  = Temp ; 
		}
		
		if(FTDebug_err_flg == false)
		{
		  	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			//中位校准成功后，存储所有中位AD值。
			//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			Write_EE_Byte((Sampling_MaxMinData[RUDDER][MIDDAT] >> 8)     , RUD_MIDVALUE_ADDR);
			Write_EE_Byte((Sampling_MaxMinData[RUDDER][MIDDAT] & 0xFF)   , RUD_MIDVALUE_ADDR + 1);
			Write_EE_Byte((Sampling_MaxMinData[ELEVATOR][MIDDAT] >> 8)   , ELE_MIDVALUE_ADDR);
			Write_EE_Byte((Sampling_MaxMinData[ELEVATOR][MIDDAT] & 0xFF) , ELE_MIDVALUE_ADDR + 1);
			Write_EE_Byte((Sampling_MaxMinData[AILERON][MIDDAT] >> 8)    , AIL_MIDVALUE_ADDR);
			Write_EE_Byte((Sampling_MaxMinData[AILERON][MIDDAT] & 0xFF)  , AIL_MIDVALUE_ADDR + 1);
			
			
			Sampling_OK_DelayCNT = 0 ; 
			MAXValueDebug_RightOrLeftflg = false ; 
			MenuCtrl.Sub_RunStep = 1 ; 
			
			//蜂鸣器提示通道设置成功(只有正常情况下才提示，防止覆盖其他报警提示 :比如低电压报警)
			if(RunStatus == __stNormal)   beepCmd(NormalFreCounts , __stMidADAdjustDown);
		}
		else    
		{
			if(RunStatus < __stNOAdjust)				//状态更新前需要判断状态等级，是否更高(否则不更新,不提示)
			{
			  	RunStatus = __stNOAdjust ;
				beepCmd(NormalFreCounts , __stFastContinumWarning);
			}
			
			//跳转到 严重故障(死在那里)
			MenuCtrl.RunStep	= __stError ; 
			MenuCtrl.Sub_RunStep 	=  0 ; 
		}
	}
}

//==============================================================================
//中位校准完毕后，稍作延时(校准状态显示)，跳转到极值校准
//==============================================================================
static void FT_SkipDelay(void)
{
	if(Sampling_OK_DelayCNT < 1000) ++Sampling_OK_DelayCNT;
	else
	{
		Sampling_OK_DelayCNT = 0 ; 
		Init_ChannelDis(false);
		MAXValueDebug_RightOrLeftflg = false ; 
		DisLEDCnts = 0 ;
	
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//初始化各摇杆的极值(在校验前，设置为默认值)
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		for(uint8_t i = 0 ; i<4 ; i++)
		{
			Sampling_MaxMinData[i][MAXDAT] = Input_Min ; 
			Sampling_MaxMinData[i][MINDAT] = Input_Max ; 
		}
		
		//先校验左边摇杆 极值(电源指示灯闪烁 提示)
		LED_State_Shake = LED_VOLT ; 
		MenuCtrl.Sub_RunStep = 2 ; 
	}
}

//==============================================================================
//获取摇杆最大值、最小值。提示用户转动摇杆。
//当获取到有效最大最小值时，再延时5S。如果有更新，继续等待5S，如果没有，校准完成
//==============================================================================
static void  FT_GetHSKMaxMinValue(void)
{
	//======================================================================
	//LED做循环旋转 ， 提示用户摇动摇杆
	//======================================================================
	if(DisLEDCnts < 600) ++DisLEDCnts ; 
	else                  DisLEDCnts = 0 ; 
	if(DisLEDCnts < 100)  		LED_State_ON = LED_CH5_H ; 
	else if(DisLEDCnts < 200)       LED_State_ON = LED_CH5_M ; 
	else if(DisLEDCnts < 300)       LED_State_ON = LED_CH5_L ; 
	else if(DisLEDCnts < 400)       LED_State_ON = LED_CH6_L ; 
	else if(DisLEDCnts < 500)       LED_State_ON = LED_CH6_M ; 
	else                            LED_State_ON = LED_CH6_H ;
	
	//校准左摇杆最大最小值 : 注意区分 美国手/日本手
	if(MAXValueDebug_RightOrLeftflg == false)
	{
		//找出左摇杆两通道的最大、最小值
		if(RFHabit == __AmericaPlayer)
		{
		  	if(Sampling_MaxMinData[RUDDER][MAXDAT] < Sampling_Data[RUDDER])   { Sampling_MaxMinData[RUDDER][MAXDAT] = Sampling_Data[RUDDER] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[RUDDER][MINDAT] > Sampling_Data[RUDDER])   { Sampling_MaxMinData[RUDDER][MINDAT] = Sampling_Data[RUDDER] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if(Sampling_MaxMinData[THROTTLE][MAXDAT] < Sampling_Data[THROTTLE]) { Sampling_MaxMinData[THROTTLE][MAXDAT] = Sampling_Data[THROTTLE] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[THROTTLE][MINDAT] > Sampling_Data[THROTTLE]) { Sampling_MaxMinData[THROTTLE][MINDAT] = Sampling_Data[THROTTLE] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if((Sampling_MaxMinData[RUDDER][MAXDAT]   > MAXValue_Min) && (Sampling_MaxMinData[RUDDER][MINDAT]   < MINValue_Max) 
			&& (Sampling_MaxMinData[THROTTLE][MAXDAT] > MAXValue_Min) && (Sampling_MaxMinData[THROTTLE][MINDAT] < MINValue_Max))
			{
				if(SamplingMaxMinValue_StabilizeCnts < 1000) ++SamplingMaxMinValue_StabilizeCnts ; 
				else
				{
				  	//再校验右边摇杆 极值(电源指示灯常亮   对码指示灯闪烁提示)
				  	LED_State_Shake = LED_BIND ; 
					MAXValueDebug_RightOrLeftflg = true ; 
					SamplingMaxMinValue_StabilizeCnts = 0 ; 
					//蜂鸣器提示通道设置成功(只有正常情况下才提示，防止覆盖其他报警提示 :比如低电压报警)
					if(RunStatus == __stNormal)   beepCmd(NormalFreCounts , __stMidADAdjustDown);
				}
			}
			else
			{
				SamplingMaxMinValue_StabilizeCnts = 0 ; 
			}
		}
		else
		{
		  	if(Sampling_MaxMinData[RUDDER][MAXDAT] < Sampling_Data[RUDDER])   { Sampling_MaxMinData[RUDDER][MAXDAT] = Sampling_Data[RUDDER] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[RUDDER][MINDAT] > Sampling_Data[RUDDER])   { Sampling_MaxMinData[RUDDER][MINDAT] = Sampling_Data[RUDDER] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if(Sampling_MaxMinData[ELEVATOR][MAXDAT] < Sampling_Data[ELEVATOR]) { Sampling_MaxMinData[ELEVATOR][MAXDAT] = Sampling_Data[ELEVATOR] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[ELEVATOR][MINDAT] > Sampling_Data[ELEVATOR]) { Sampling_MaxMinData[ELEVATOR][MINDAT] = Sampling_Data[ELEVATOR] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if((Sampling_MaxMinData[RUDDER][MAXDAT]   > MAXValue_Min) && (Sampling_MaxMinData[RUDDER][MINDAT]   < MINValue_Max) 
			&& (Sampling_MaxMinData[ELEVATOR][MAXDAT] > MAXValue_Min) && (Sampling_MaxMinData[ELEVATOR][MINDAT] < MINValue_Max))
			{
				if(SamplingMaxMinValue_StabilizeCnts < 1000) ++SamplingMaxMinValue_StabilizeCnts ; 
				else
				{
				  	//再校验右边摇杆 极值(电源指示灯常亮   对码指示灯闪烁提示)
				  	LED_State_Shake = LED_BIND ; 
					MAXValueDebug_RightOrLeftflg = true ; 
					SamplingMaxMinValue_StabilizeCnts = 0 ;
					//蜂鸣器提示通道设置成功(只有正常情况下才提示，防止覆盖其他报警提示 :比如低电压报警)
					if(RunStatus == __stNormal)   beepCmd(NormalFreCounts , __stMidADAdjustDown);
				}
			}
			else
			{
				SamplingMaxMinValue_StabilizeCnts = 0 ; 
			}
		}
	}
	else
	{
	  	LED_State_ON |= LED_VOLT ;
		//找出左摇杆两通道的最大、最小值
		if(RFHabit == __AmericaPlayer)
		{
		  	if(Sampling_MaxMinData[AILERON][MAXDAT] < Sampling_Data[AILERON])   { Sampling_MaxMinData[AILERON][MAXDAT] = Sampling_Data[AILERON] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[AILERON][MINDAT] > Sampling_Data[AILERON])   { Sampling_MaxMinData[AILERON][MINDAT] = Sampling_Data[AILERON] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if(Sampling_MaxMinData[ELEVATOR][MAXDAT] < Sampling_Data[ELEVATOR]) { Sampling_MaxMinData[ELEVATOR][MAXDAT] = Sampling_Data[ELEVATOR] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[ELEVATOR][MINDAT] > Sampling_Data[ELEVATOR]) { Sampling_MaxMinData[ELEVATOR][MINDAT] = Sampling_Data[ELEVATOR] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if((Sampling_MaxMinData[AILERON][MAXDAT]  > MAXValue_Min) && (Sampling_MaxMinData[AILERON][MINDAT]  < MINValue_Max) 
			&& (Sampling_MaxMinData[ELEVATOR][MAXDAT] > MAXValue_Min) && (Sampling_MaxMinData[ELEVATOR][MINDAT] < MINValue_Max))
			{
				if(SamplingMaxMinValue_StabilizeCnts < 1000) ++SamplingMaxMinValue_StabilizeCnts ; 
				else
				{
					LED_State_ON = LED_BIND ;  
					MenuCtrl.Sub_RunStep = 3 ; 
					Sampling_OK_DelayCNT = 0 ; 
					//蜂鸣器提示通道设置成功(只有正常情况下才提示，防止覆盖其他报警提示 :比如低电压报警)
					if(RunStatus == __stNormal)   beepCmd(NormalFreCounts , __stMidADAdjustDown);
				}
			}
			else
			{
				SamplingMaxMinValue_StabilizeCnts = 0 ; 
			}
		}
		else
		{
		  	if(Sampling_MaxMinData[AILERON][MAXDAT] < Sampling_Data[AILERON])   { Sampling_MaxMinData[AILERON][MAXDAT] = Sampling_Data[AILERON] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[AILERON][MINDAT] > Sampling_Data[AILERON])   { Sampling_MaxMinData[AILERON][MINDAT] = Sampling_Data[AILERON] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if(Sampling_MaxMinData[THROTTLE][MAXDAT] < Sampling_Data[THROTTLE]) { Sampling_MaxMinData[THROTTLE][MAXDAT] = Sampling_Data[THROTTLE] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			if(Sampling_MaxMinData[THROTTLE][MINDAT] > Sampling_Data[THROTTLE]) { Sampling_MaxMinData[THROTTLE][MINDAT] = Sampling_Data[THROTTLE] ; SamplingMaxMinValue_StabilizeCnts = 0 ; }
			
			if((Sampling_MaxMinData[AILERON][MAXDAT]  > MAXValue_Min)  && (Sampling_MaxMinData[AILERON][MINDAT]  < MINValue_Max) 
			&& (Sampling_MaxMinData[THROTTLE][MAXDAT] > MAXValue_Min)  && (Sampling_MaxMinData[THROTTLE][MINDAT] < MINValue_Max))
			{
				if(SamplingMaxMinValue_StabilizeCnts < 1000) ++SamplingMaxMinValue_StabilizeCnts ; 
				else
				{
				  	LED_State_ON = LED_BIND ;  
					MenuCtrl.Sub_RunStep = 3 ; 
					Sampling_OK_DelayCNT = 0 ; 
					//蜂鸣器提示通道设置成功(只有正常情况下才提示，防止覆盖其他报警提示 :比如低电压报警)
					if(RunStatus == __stNormal)   beepCmd(NormalFreCounts , __stMidADAdjustDown);
				}
			}
			else
			{
				SamplingMaxMinValue_StabilizeCnts = 0 ; 
			}
		}
	}
}

//==============================================================================
//工厂校准成功后，提示成功(显示2S，再跳转)
//跳转到启动阶段
//==============================================================================
static void FT_OK(void)
{
	if(Sampling_OK_DelayCNT < 1000) ++Sampling_OK_DelayCNT;
	else
	{
		Sampling_OK_DelayCNT = 0 ; 
		//======================================
		//  存储所有校准极值，并且重置偏置值
		//======================================
		Write_EE_Byte(IS_EVER_MIDCALIBRATION_FLG , IS_EVER_MIDCALIBRATION_ADDR);
					
		Write_EE_Byte((Sampling_MaxMinData[RUDDER][MAXDAT] >> 8)     , RUD_MAXVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[RUDDER][MAXDAT] & 0xFF)   , RUD_MAXVALUE_ADDR + 1);
		Write_EE_Byte((Sampling_MaxMinData[RUDDER][MINDAT] >> 8)     , RUD_MINVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[RUDDER][MINDAT] & 0xFF)   , RUD_MINVALUE_ADDR + 1);
					
		Write_EE_Byte((Sampling_MaxMinData[THROTTLE][MAXDAT] >> 8)     , THR_MAXVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[THROTTLE][MAXDAT] & 0xFF)   , THR_MAXVALUE_ADDR + 1);
		Write_EE_Byte((Sampling_MaxMinData[THROTTLE][MINDAT] >> 8)     , THR_MINVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[THROTTLE][MINDAT] & 0xFF)   , THR_MINVALUE_ADDR + 1);				
		
		//防止操作 EEPROM ， 看门狗超时复位
		FeedTheDog();						//喂狗
		
		Write_EE_Byte((Sampling_MaxMinData[ELEVATOR][MAXDAT] >> 8)     , ELE_MAXVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[ELEVATOR][MAXDAT] & 0xFF)   , ELE_MAXVALUE_ADDR + 1);
		Write_EE_Byte((Sampling_MaxMinData[ELEVATOR][MINDAT] >> 8)     , ELE_MINVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[ELEVATOR][MINDAT] & 0xFF)   , ELE_MINVALUE_ADDR + 1);
					
		Write_EE_Byte((Sampling_MaxMinData[AILERON][MAXDAT] >> 8)     , AIL_MAXVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[AILERON][MAXDAT] & 0xFF)   , AIL_MAXVALUE_ADDR + 1);
		Write_EE_Byte((Sampling_MaxMinData[AILERON][MINDAT] >> 8)     , AIL_MINVALUE_ADDR);
		Write_EE_Byte((Sampling_MaxMinData[AILERON][MINDAT] & 0xFF)   , AIL_MINVALUE_ADDR + 1);
		
		//防止操作 EEPROM ， 看门狗超时复位
		FeedTheDog();						//喂狗
		
		Sampling_Offset[RUDDER]   = 50 ; 
		Write_EE_Byte(50   , RUD_OFFSET_ADDR);
		Sampling_Offset[THROTTLE] = 50 ; 
		Write_EE_Byte(50    , THR_OFFSET_ADDR);
		Sampling_Offset[ELEVATOR] = 50 ; 
		Write_EE_Byte(50   , ELE_OFFSET_ADDR);
		Sampling_Offset[AILERON]  = 50 ; 
		Write_EE_Byte(50   , AIL_OFFSET_ADDR);
		
		
		//打开通道显示(更新 LED 显示)
		Init_ChannelDis(true);
		LED_State_ON    |= LED_BIND   ; 
		MenuCtrl.RunStep = __stSarttUp ;
		MenuCtrl.Sub_RunStep = 0 ; 
	}
}

static void(*pFT[])(void) = 
{
	FT_GetHSKMidValue ,
	FT_SkipDelay , 
	FT_GetHSKMaxMinValue , 
	FT_OK , 
};

/*==============================================================================
(1)先标定三摇杆的中位值(除油门外的其他三个摇杆)
方法 : 每个摇杆记录10个值，去掉高位，低位，其余8个取平均。
==============================================================================*/
void FTDebug(void)
{
	pFT[MenuCtrl.Sub_RunStep]();
}
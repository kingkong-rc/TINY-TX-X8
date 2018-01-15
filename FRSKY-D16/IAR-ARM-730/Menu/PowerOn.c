/*******************************************************************************
上电检测 : 
(1)检测反向拨码开关位置。
(2)是否需要进入工厂校准模式。
*******************************************************************************/
#include "include.h"

//上电延时 1000 mS 等待 电池电量 、油门摇杆位置 、 按键检测完成
static uint16_t PowerOn_Counts = 0 ; 

void PowerOn(void)
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//每次上电时检测一次，中间更换反向设置需要等待下一次上电有效。
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	InversionKey_Scan() ;  
	
	//上电延时计数
	if(PowerOn_Counts < 500) ++PowerOn_Counts ; 
	else
	{
	  	uint8_t KeyTemp = 0 ; 
		
		//AUTO 和 CH6_DOWN 同时按下，进入中位校准程序
		KeyTemp  = (GPIOE -> IDR & ((1<<4) | (1<<6))) ; 
		//进入工厂校准
		if(((KeyTemp == 0x00)) || (FTDebug_err_flg == true))		//右边五维 Enter 按键 下 + CH6_Down同时按下 或者 上电时中位校准数据无效。进入中位校准状态
		{
			//进入工厂校准前，关闭所有LED显示 
			Init_ChannelDis(false);
			
			MenuCtrl.RunStep =  __stFTDebug;
			MenuCtrl.Sub_RunStep = 0 ; 
		}
		
		//无按键 / 无效按键 且中位值校准正常，启动 
		else
		{
		  	//显示当前通道设置情况(AUX1 - AUX2)
		  	Init_ChannelDis(true);
			LED_State_Shake &= ~LED_BIND ;
			LED_State_ON    |=  LED_BIND ; 
			
			MenuCtrl.RunStep =  __stSarttUp;
			MenuCtrl.Sub_RunStep = 0 ; 
		}
	}
}
#include "include.h"

#define FOSC  12000000
#define BAUD  115200

uint8_t LED_State_ON     = 0x00 ;						//LED常亮
uint8_t LED_State_Shake  = 0x00 ;						//LED闪烁

uint8_t LED_Status_SendDat = 0 ; 

//==============================================================================
//LED控制初始化(UART1_TX)
//==============================================================================
void LED_UART1_Init(void)
{
  	//打开USART1 时钟(低功耗STM8才需要)
  	CLK -> PCKENR1 |= (1<<5) ;
	
  	USART_DeInit(USART1);
	
  	//设置串口1: 115200    12M 
	uint16_t BRR_Counts = 0 ; 
	BRR_Counts = FOSC / BAUD ; 
	USART1 -> BRR2  =  BRR_Counts & 0x000F ; 
       	USART1 -> BRR2 |= ((BRR_Counts & 0xf000) >> 8);
    	USART1 -> BRR1  = ((BRR_Counts & 0x0ff0) >> 4);     			/*先给BRR2赋值 最后再设置BRR1*/  
	
	USART1 -> CR1  |= (1<<6)|(1<<4)|(1<<2)|(1<<1);				//9位数据 1位停止位 奇校验 无中断
	USART1 -> CR2  |= (1<<3) ;						//发送使能
	USART1 -> CR3  = 0 ; 
	
	USART1->CR1 &= (uint8_t)(~USART_CR1_USARTD);
}



//==============================================================================
//串口发送3Byte 数据给灯板，其中 1Byte 数据头 + 1Byte显示数据 + 1Byte显示数据取反
//显示数据 (8Bit) : 分别对应显示的8个LED
//灯板收到数据后，需要先判断数据有效性。有效更改LED显示状态，无效继续上一个状态显示
//发送频率 : 5mS/次       LED闪烁频率50mS/次(其中电源指示灯不受此闪烁频率限制)
//==============================================================================
void LED_Usart1Send(void)
{
	static uint16_t Shake_Rate_Counts = 0 ; 
	
	
	if(Shake_Rate_Counts < 40) ++Shake_Rate_Counts ;
	else Shake_Rate_Counts = 0 ; 
	
	
	if(Shake_Rate_Counts < 20)
	{
	  	//静态显示 : 只点亮 常亮部分LED
		LED_Status_SendDat = LED_State_ON ; 
	}
	else
	{	
	  	//闪烁显示 : 点亮 常亮部分 + 闪烁部分
	  	LED_Status_SendDat = LED_State_ON | LED_State_Shake ; 
	}
	
  	while(!(USART1 -> SR & (1<<7))); USART_SendData8(USART1, 0x55); 
	while(!(USART1 -> SR & (1<<7))); USART_SendData8(USART1, LED_Status_SendDat); 
	while(!(USART1 -> SR & (1<<7))); USART_SendData8(USART1, ~LED_Status_SendDat);   
}


/*==============================================================================
锂电池 30% - 100% 电压 : 高于3.77V。             常亮  
       10% - 30%  电压 : 3.65V  - 3.77V  1S      慢闪
       0%  - 10%  电压 : 低于 3.65V      0.25S   快闪

电源指示灯显示 : 显示电池电量情况(电量越低，闪烁越快，直至常亮)
==============================================================================*/
typedef enum 
{
	__stAbove30Percent 	= 1 , 
	__stAbove10Percent	= 2 , 
	__stUnder10Percent 	= 3 , 
}VoltStatusTypeDef ; 
void Volt_LED(float Volt_Value)
{
  	//电池电量检测 : 为防止检测电压处于临界点时，上下两级来回跳动，先加入防干扰处理
  	//电压需要连续采样超过 2S 处于同一状态，认为数据有效。
	static VoltStatusTypeDef VoltStatus = __stAbove30Percent ;
	static uint16_t VoltStatusUpCNT   = 0 ; 
	static uint16_t VoltStatusDownCNT = 0 ; 
	
	//电源指示灯闪烁频率计数
  	static uint8_t VoltLED_ShakeRateCnt = 0 ; 
	static uint8_t VoltLED_Counts = 0 ;  
	
	//======================================================================
	//电池电量检测判断:默认电压只会一直下降(不考虑上升情况)
	//(1)30% - 100%电量 : 检测电压 小于 3.77V 持续3S 电量降低至 10% - 30% 状态   
	//(2)10% - 30% 电量 : 检测电压 小于 3.65V 持续3S 电量降低至 10%         大于 3.79V 持续3S ，认为电量充电 到 30% - 100%状态
	//(3)0%  - 10% 电量 : 检测电压 大于 3.68V 认为电量充电 到   10% - 30%状态
	//======================================================================
	if(VoltStatus == __stAbove30Percent) 
	{
	  	//当电压小于3.77V时，认为电量降到了 10% - 30%
		if(Volt_Value < 3.77f) 
		{
			if(VoltStatusDownCNT < 400) ++VoltStatusDownCNT ; 
			else 
			{
				VoltStatus = __stAbove10Percent ; 
				VoltStatusDownCNT = 0 ; 
			}
		}
		else VoltStatusDownCNT = 0 ;
		
		VoltStatusUpCNT = 0 ; 						//清零(电量大于30% 不需要考虑 电量再上升情况)
	}
	else if(VoltStatus == __stAbove10Percent) 
	{
	  	//当电压升到 3.79V时 ，才认为电量升到了 30%(为了避免电压波动造成的状态不稳定)
		if(Volt_Value > 3.79f) 
		{
		  	VoltStatusDownCNT = 0 ; 
			if(VoltStatusUpCNT < 400) ++VoltStatusUpCNT ; 
			else 
			{
				VoltStatus = __stAbove30Percent ; 
				VoltStatusUpCNT = 0 ;
			}
		}
		
		//当电压小于 3.65V 认为电量低于 10 %
		else if(Volt_Value < 3.65f) 
		{
		  	VoltStatusUpCNT = 0 ; 
			if(VoltStatusDownCNT < 400) ++VoltStatusDownCNT ; 
			else 
			{
				VoltStatus = __stUnder10Percent ; 
				VoltStatusDownCNT = 0 ;
			}
		}
		
		//电池电压 在 (3.65 - 3.79V 之间 )
		else
		{
		  	VoltStatusUpCNT = 0 ; 
			VoltStatusDownCNT = 0 ;
		}
	}
	else
	{
		//当电池电压大于 3.68，才认为电量升到10%以上
	  	if(Volt_Value > 3.68f) 
		{
			if(VoltStatusUpCNT < 400) ++VoltStatusUpCNT ; 
			else
			{
				VoltStatus = __stAbove10Percent ; 
				VoltStatusUpCNT = 0 ;
			}
		}
		else
		{
			VoltStatusUpCNT = 0 ;
		}
		  	 
		VoltStatusDownCNT = 0 ;
	}
	
	
	//======================================================================
	//根据电量状态，设置电池指示灯显示(常亮 / 慢闪 / 快闪)
	//======================================================================
	if(VoltStatus == __stAbove30Percent)
	{
	  	VoltLED_Counts = 0 ; 		//30% - 100% : 常亮
		//电量回升后，清除低压报警
		if(RunStatus == __stVolt_err)
		{
			beepCmd(NormalFreCounts , __stStop);
			RunStatus = __stNormal ;
		}
		
	}
	else if(VoltStatus == __stAbove10Percent)
	{
		VoltLED_Counts = 200 ; 		//10% - 30% : 慢闪
		//电量回升后，清除低压报警
		if(RunStatus == __stVolt_err)
		{
			beepCmd(NormalFreCounts , __stStop);
			RunStatus = __stNormal ;
		}
	}
	else 
	{
	  	VoltLED_Counts = 50 ;		//低于30%   : 快闪
		//低电量报警
		if(RunStatus < __stVolt_err)
		{
		  	RunStatus = __stVolt_err ;
			beepCmd(NormalFreCounts , __stFastContinumWarning);
		}
	}
	
	if(VoltLED_ShakeRateCnt < VoltLED_Counts) ++VoltLED_ShakeRateCnt ; 
	else VoltLED_ShakeRateCnt = 0 ; 
	
	if(VoltLED_ShakeRateCnt <= (VoltLED_Counts/2))
	{
		LED_State_ON |= LED_VOLT ; 	//关闭  电源指示灯
	}
	else
	{
		LED_State_ON &= ~LED_VOLT ; 	//打开 电源指示灯
	}
}
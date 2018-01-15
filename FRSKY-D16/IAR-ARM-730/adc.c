/*******************************************************************************
--------------------------------------------------------------------------------
ADC通道对应功能 : 
ADC11(PB7) -> Random          ADC13(PB5) -> RUD          ADC14(PB4) -> THR(ELE)
ADC15(PB3) -> ELE(THR)        ADC16(PB2) -> AIL          ADC17(PB1) -> BAT
ADC18(PB0) -> REF             
内部参考电
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
*******************************************************************************/
#include "include.h"

#define  ADC1_DR_Address                    (ADC1_BASE + 0x04)
#define  ADC1_Buffer_Size                    8

uint16_t ADC_Value[30] ;							//ADC DMA 内存缓存区		
bool     ISCJ431OKFlg = true ; 							//431 状态标志位(1: 正常   0: 异常)
float    VoltValue = 0.0f ;							//电池电压


//==============================================================================
//重置 ADC
//(1)如果ADC溢出(ADC值可能已经错位)，重新初始化 DMA ADC。
//(2)使能单次ADC转换。
//==============================================================================
void StartupAgain_ADC(void)
{
  	//判断ADC是否有溢出
	if(ADC1 -> SR & (1<<2))
	{
		ADC1 -> SR &= ~(1<<2) ; 					//清零溢出标志位
		
		//重置 ADC  DMA 
		ADC1 -> CR1 &= ~(1<<1) ; 					//先关闭ADC
		ADC_DMACmd(ADC1,DISABLE); 					//禁止 ADC DMA请求
		DMA_DeInit(DMA1_Channel0);
		DMA_Init( DMA1_Channel0 , (uint32_t)ADC_Value,			//DMA内存地址
                                  ADC1_DR_Address,				//DMA ADC外设地址
				  ADC1_Buffer_Size,				//传输数据个数 : 8
				  DMA_DIR_PeripheralToMemory,			//传输方向 : 外设 -> 内存
				  DMA_Mode_Circular,				//DMA模式 : 连续传输
                                  DMA_MemoryIncMode_Inc,			//内存地址累加
				  DMA_Priority_High,				//DMA优先级 : 高
				  DMA_MemoryDataSize_HalfWord );		//传输数据尺寸 : 16 bit
		DMA_Cmd(DMA1_Channel0,ENABLE);
		
		ADC_DMACmd(ADC1,ENABLE);	//使能ADC  DMA
	}
	//使能单次ADC 转换 
	ADC1 -> CR1 |= (1<<1) ;
}

//==============================================================================
//初始化 ADC  DMA_CH0
//==============================================================================
void adc_Init(void)
{
	//初始化GPIO 浮空输入(共 7 通道  加上内不参考电   共8通道)
  	GPIOB -> CR1 &= ~((1<<7)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0));
	GPIOB -> CR2 &= ~((1<<7)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0));
	GPIOB -> DDR &= ~((1<<7)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0));
	CLK -> PCKENR2 |= (1<<4)|(1<<0); 					//打开 ADC1  DMA1 时钟 
	
	//初始化 DMA1_CH0 
	DMA_DeInit(DMA1_Channel0);
	DMA_Init( DMA1_Channel0 , (uint32_t)ADC_Value,				//DMA内存地址
                                  ADC1_DR_Address,				//DMA ADC外设地址
				  ADC1_Buffer_Size,				//传输数据个数 : 8
				  DMA_DIR_PeripheralToMemory,			//传输方向 : 外设 -> 内存
				  DMA_Mode_Circular,				//DMA模式 : 连续传输
                                  DMA_MemoryIncMode_Inc,			//内存地址累加
				  DMA_Priority_High,				//DMA优先级 : 高
				  DMA_MemoryDataSize_HalfWord );		//传输数据尺寸 : 16 bit
	DMA_Cmd(DMA1_Channel0,ENABLE);
	DMA_GlobalCmd(ENABLE);
		
        ADC1 -> CR1 = (1<<0);       						//分辨率 : 12bit  关闭所有ADC相关中断  单次转换  使能ADC                              		
        ADC1 -> CR2 = (1<<2)|(1<<1);						//设置前24通道    采样率 192ADC Clock cycles
	ADC1 -> CR3 = (1<<7)|(1<<6);						//设置内部参考点  采样率 192ADC Clock cycles
	ADC_VrefintCmd(ENABLE);
	
	//关闭外部 7 通道 GPIO 施密特触发器
	ADC_SchmittTriggerConfig(ADC1, ADC_Channel_11, DISABLE) ; 
	ADC_SchmittTriggerConfig(ADC1, ADC_Channel_13, DISABLE) ; 
	ADC_SchmittTriggerConfig(ADC1, ADC_Channel_14, DISABLE) ; 
	ADC_SchmittTriggerConfig(ADC1, ADC_Channel_15, DISABLE) ; 
	ADC_SchmittTriggerConfig(ADC1, ADC_Channel_16, DISABLE) ; 
	ADC_SchmittTriggerConfig(ADC1, ADC_Channel_17, DISABLE) ; 
	ADC_SchmittTriggerConfig(ADC1, ADC_Channel_18, DISABLE) ; 
	
	//使能所有 8 通道 ADC(7个外部GPIO通道 + 1个内部参考电通道)
	ADC_ChannelCmd(ADC1, ADC_Channel_11, ENABLE) ; 
	ADC_ChannelCmd(ADC1, ADC_Channel_13, ENABLE) ; 
	ADC_ChannelCmd(ADC1, ADC_Channel_14, ENABLE) ; 
	ADC_ChannelCmd(ADC1, ADC_Channel_15, ENABLE) ; 
	ADC_ChannelCmd(ADC1, ADC_Channel_16, ENABLE) ; 
	ADC_ChannelCmd(ADC1, ADC_Channel_17, ENABLE) ; 
	ADC_ChannelCmd(ADC1, ADC_Channel_18, ENABLE) ; 
	ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, ENABLE) ; 
	
	SYSCFG -> RMPCR1 = 0x0C ; 						//映射 DMA1 Channel0 -> ADC1 
	ADC_DMACmd(ADC1,ENABLE);
	ADC1 -> CR1 |= (1<<1) ;							//开始一次 ADC  
}

//====================================================================================================================
// ADC_Value[1]     -> RUD      ADC_Value[2]    -> THR         ADC_Value[3]     -> ELE        ADC_Value[4]     -> AIL      
// ADC_Value[5]     -> BAT      ADC_Value[6]    -> CJ_431      ADC_Value[7]     -> IR_REF     ADC_Value[0]     -> Random    
// 为防止AD值来回跳动 ，对所有 AD 值 做平滑滤波
//====================================================================================================================
void GetADValue(void)
{
	////////////////////////////////////////////////////////////////////////
	//单片机内部参考电ADC(用于判断CJ431是否正常， CJ431不正常时用内部参考电做基准) 
	//STM8L052R8内部基准电压 : 1.224V * (4095/3.3V) ≈ 1519
	////////////////////////////////////////////////////////////////////////
	static uint16_t IRV_ADValue  = 1519 ; 				
	static uint16_t ADJ_ADValue  = 3102 ; 				        //CJ431基准电压  2.5V * (4095/3.3V) 
	static uint16_t Volt_ADValue = 3309 ; 				        //初始化电池电压 4.0V * (2/3)* (4095/3.3V)
		
	uint16_t TempAD = 0 ; 
	uint16_t LimitTemp = 0 ; 
	
	//计算内部参考电(1.224V)
	TempAD =  ADC_Value[7] ;
	if(IRV_ADValue > TempAD)  LimitTemp = IRV_ADValue - TempAD ;  
	else			  LimitTemp = TempAD - IRV_ADValue ;  
	if(LimitTemp > 30)//变化超过一定范围，立即响应
	{
		IRV_ADValue = TempAD ; 
	}
	else
	{
		IRV_ADValue = (uint16_t)(IRV_ADValue*0.8f + TempAD *0.2f) ;
	}
	if((IRV_ADValue > 1719) || (IRV_ADValue < 1319)) IRV_ADValue  = 1519 ;
		
	//计算出CJ431的比例系数 
	TempAD =  ADC_Value[6] ;
	if(ADJ_ADValue > TempAD)  LimitTemp = ADJ_ADValue - TempAD ;  
	else			  LimitTemp = TempAD - ADJ_ADValue ;  
	if(LimitTemp > 30)//变化超过一定范围，立即响应
	{
		ADJ_ADValue = TempAD ; 
	}
	else
	{
		ADJ_ADValue = (uint16_t)(ADJ_ADValue*0.8f + TempAD *0.2f) ;
	}
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//内部参考电用来判断 CJ431参考电的有效性。如果CJ431判定无效，
	//则使用内部参考电来计算其余AD值
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	TempAD = (uint16_t)((1519.0f/IRV_ADValue)*ADJ_ADValue) ; 
	if(TempAD < 3002)        { ISCJ431OKFlg = false ; ADJ_ADValue = 3102 ;} //因为参考电压AD值后面用于作为被除数(不能等于零,否则程序跑飞)
	else if(TempAD > 3202)   { ISCJ431OKFlg = false ; ADJ_ADValue = 3102 ;}
		
	//计算电池电压
	TempAD =  ADC_Value[5] ;
	if(ISCJ431OKFlg == true)TempAD = (uint16_t)(TempAD * ( 3102 / (float)ADJ_ADValue)) ;
	else			TempAD = (uint16_t)(TempAD * ( 1519 / (float)IRV_ADValue)) ;
	if(Volt_ADValue > TempAD)  LimitTemp = Volt_ADValue - TempAD ;  
	else			   LimitTemp = TempAD - Volt_ADValue ;  
	if(LimitTemp > 30)
	{
		Volt_ADValue = TempAD ; 
	}
	else
	{
		Volt_ADValue = (uint16_t)(Volt_ADValue * 0.8f + TempAD * 0.2f) ; 	
	}
	VoltValue =  (float)Volt_ADValue/Input_Max *3.3f* 1.51f ;		//5.1K + 10K分压(锂电池电压)
		
	//计算RUDDER
	TempAD = ADC_Value[1] ;
	if(ISCJ431OKFlg == true) TempAD = (uint16_t)(TempAD * ( 3102 / (float)ADJ_ADValue)) ;
	else			 TempAD = (uint16_t)(TempAD * ( 1519 / (float)IRV_ADValue)) ;
	if(Sampling_Data[RUDDER] > TempAD)  	LimitTemp = Sampling_Data[RUDDER] - TempAD  ;  
	else			   		LimitTemp = TempAD - Sampling_Data[RUDDER]  ;  
	if(LimitTemp < 20)
	{
		TempAD = (uint16_t)(Sampling_Data[RUDDER] * 0.8f + TempAD * 0.2f); 	
	}
	if(TempAD > Input_Max) 	Sampling_Data[RUDDER]   = Input_Max ; 
	else              	Sampling_Data[RUDDER]   = TempAD ;
		
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!  注意 : 美国手和日本手 THROTTLE 和 ELEVATOR 互换   !!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//计算Throttle
	if(RFHabit == __AmericaPlayer)
	{
		TempAD =  ADC_Value[2] ;
	}
	else
	{
		TempAD =  ADC_Value[3] ;
	}
	if(ISCJ431OKFlg == true) TempAD = (uint16_t)(TempAD * ( 3102 / (float)ADJ_ADValue)) ;
	else			 TempAD = (uint16_t)(TempAD * ( 1519 / (float)IRV_ADValue)) ;
	if(Sampling_Data[THROTTLE] > TempAD)  	LimitTemp = Sampling_Data[THROTTLE] - TempAD ;  
	else			   		LimitTemp = TempAD - Sampling_Data[THROTTLE] ;  
	if(LimitTemp < 20)
	{
		TempAD = (uint16_t)(Sampling_Data[THROTTLE] * 0.8f + TempAD * 0.2f); 	
	}
	if(TempAD > Input_Max) 	Sampling_Data[THROTTLE]   = Input_Max ; 
	else              	Sampling_Data[THROTTLE]   = TempAD ;
		
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!  注意 : 美国手和日本手 THROTTLE 和 ELEVATOR 互换   !!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//计算ELEVATOR
	if(RFHabit == __AmericaPlayer)
	{
		TempAD =  ADC_Value[3] ;
	}
	else
	{
		TempAD =  ADC_Value[2] ;
	}
	if(ISCJ431OKFlg == true) TempAD = (uint16_t)(TempAD * ( 3102 / (float)ADJ_ADValue)) ;
	else			 TempAD = (uint16_t)(TempAD * ( 1519 / (float)IRV_ADValue)) ;
	if(Sampling_Data[ELEVATOR] > TempAD)  	LimitTemp = Sampling_Data[ELEVATOR] - TempAD ;  
	else			   		LimitTemp = TempAD - Sampling_Data[ELEVATOR] ;  
	if(LimitTemp < 20)
	{
		TempAD = (uint16_t)(Sampling_Data[ELEVATOR] * 0.8f + TempAD * 0.2f); 	
	}
	if(TempAD > Input_Max) 	Sampling_Data[ELEVATOR]   = Input_Max ; 
	else              	Sampling_Data[ELEVATOR]   = TempAD ;
		
	//计算AILERON
	TempAD =  ADC_Value[4] ;
	if(ISCJ431OKFlg == true) TempAD = (uint16_t)(TempAD * ( 3102 / (float)ADJ_ADValue)) ;
	else			 TempAD = (uint16_t)(TempAD * ( 1519 / (float)IRV_ADValue)) ;
	if(Sampling_Data[AILERON] > TempAD)  	LimitTemp = Sampling_Data[AILERON] - TempAD ;  
	else			   		LimitTemp = TempAD - Sampling_Data[AILERON] ;  
	if(LimitTemp < 20)
	{
		TempAD = (uint16_t)(Sampling_Data[AILERON] * 0.8f + TempAD * 0.2f); 	
	}
	if(TempAD > Input_Max) 	Sampling_Data[AILERON]   = Input_Max ; 
	else              	Sampling_Data[AILERON]   = TempAD ;
		
	//更新摇杆部分所有无线发送通道值
	//tx________________Value_____________________Test ();
	FRSKYD16_SendDataBuff[RUDDER]   = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[RUDDER]);  
	FRSKYD16_SendDataBuff[THROTTLE] = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[THROTTLE]); 
	FRSKYD16_SendDataBuff[ELEVATOR] = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[ELEVATOR]);  
	FRSKYD16_SendDataBuff[AILERON]  = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[AILERON]); 
	//开关通道
	FRSKYD16_SendDataBuff[AUX1]     = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[AUX1]);  
	FRSKYD16_SendDataBuff[AUX2]     = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[AUX2]);  
	FRSKYD16_SendDataBuff[AUX3]     = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[AUX3]);  
	FRSKYD16_SendDataBuff[AUX4]     = Get_SendValue((ChannelTypeDef)FRSKYD16_CH_Code[AUX4]);
}




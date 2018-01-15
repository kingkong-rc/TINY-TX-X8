/*
2017-8-16 : 取消 USART3 DMA发送完成中断。现在改为每次发送一包数据后停止，下次发送前先关闭再打开(因为会和 TIM2 无线发送中断相互冲突，影响无线发送时基)
*/
#include "include.h"

#define  BAUD  115200
#define  Fosc  12000000

#define  U3_DR_Address                    (USART3_BASE + 0x01)
#define  U3_Buffer_Size 		   103

bool DMA1_U3_Tx_Flag = false ; 		  //DMA发送标志位

static uint8_t U3_DMA_TXBuff[U3_Buffer_Size] = 
{
  	//起始头(7Byte  数据固定)
	0x4C , 0x44 , 0x41 , 0x54 , 0x58 , 0x31 , 0x30 , 
	//遥控器类型(1Byte  0x45 / 0x44)
	0x45 , 
	//遥控器软件版本号(3Byte  年 - 月 - 日)
	0x00 , 0x00 , 0x00 , 
	//无线协议版本号(1Byte)
	0x00 ,
	//美国手/日本手模式选择
	0x00 ,
	//遥控器唯一ID号(4Byte)
	0x00 , 0x00 , 0x00 , 0x00 , 
	//16通道数据(每通道2Byte = 16*2Byte = 32Byte)
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 ,
	//通道反向标志位(1Byte)
	0x00 , 
	//所有按键 + 8位拨码开关GPIO值(2Byte)
	0x00 , 0x00 , 0x00 , 
	//电池电压值(2Byte  电池电压放大100倍)
	0x00 , 0x00 , 
	//参考电431 + 高频模块 + 中位校准是否成功  标志位(1Byte)
	0x00 , 
	//所有通道AD采样原始值(每通道2Byte = 6*2Byte = 12Byte)
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 
	//LED状态(1Byte)  1代表点亮  0代表熄灭
	0x00 , 
	//报警状态(1Byte)
	0x00 , 
	//定时器死机次数(1Byte)
	0x00 , 
	//四摇杆偏置值(4Byte)
	0x00 , 0x00 , 0x00 , 0x00 , 
	
	//四摇杆最大AD值(4*2Byte = 8Byte)
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 
	
	//四摇杆中位AD值(4*2Byte = 8Byte)
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 
	
	//四摇杆最小AD值(4*2Byte = 8Byte)
	0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 0x00 , 
	
	//数据包校验和(2Byte)
	0x00 , 0x00 , 
}; 


void U3_Init(void)
{
  	CLK -> PCKENR3 |= (1<<4) ; 						//打开 usart3 时钟 
	
	//初始化 DMA1_CH1 (Usart3_TX DMA)
	DMA_DeInit(DMA1_Channel1);
	DMA_Init( DMA1_Channel1 , (uint32_t)U3_DMA_TXBuff,			//DMA内存地址
                                  U3_DR_Address,				//DMA ADC外设地址
				  U3_Buffer_Size,				//传输数据个数 : 103
				  DMA_DIR_MemoryToPeripheral,			//传输方向 : 内存 -> 外设
				  DMA_Mode_Normal,				//DMA模式 : 单次传输
                                  DMA_MemoryIncMode_Inc,			//内存地址累加
				  DMA_Priority_High,				//DMA优先级 : 高
				  DMA_MemoryDataSize_Byte );			//传输数据尺寸 : 8 bit
	DMA_Cmd(DMA1_Channel1,DISABLE);						//先关闭DMA(需要时再打开)
	DMA_GlobalCmd(ENABLE);
	
	//Usart3初始化
	uint16_t BRR_Counts = Fosc / BAUD ; 
	
	USART3 -> BRR2  =  BRR_Counts & 0x000F ; 
       	USART3 -> BRR2 |= ((BRR_Counts & 0xf000) >> 8);
    	USART3 -> BRR1  = ((BRR_Counts & 0x0ff0) >> 4);     			/*先给BRR2赋值 最后再设置BRR1*/  
	USART3 -> CR2   = (1<<3) ; 						//发送使能
	USART3 -> CR3   = 0 ; 
	USART3 -> CR1 &= ~(1<<5);						//使能串口
	
	DMA1_U3_Tx_Flag = false ; 
}


//==============================================================================
//发送串口数据包
//==============================================================================
void U3_DMATX_ONOFF(void)
{
  	//发送完成标志位
  	if (DMA1_Channel1 -> CSPR & (1<<1))
	{
		DMA1_Channel1 -> CSPR &= ~(1<<1) ; 	//清除中断标记
		USART3 -> CR5   &= ~(1<<7) ; 		//非常重要 关闭UART1 DMA发送请求
		DMA1_Channel1->CCR &= ~(1<<0);		//关闭DMA
		DMA1_U3_Tx_Flag = false ; 		//置位标志位
	}
  
  	//发送完成后，才会开始发送
    	if (DMA1_U3_Tx_Flag == false)
	{
		//设置发送个数
		DMA1_Channel1 -> CNBTR = U3_Buffer_Size;
		
		//启动UART3 DMA发送！
		USART3 -> CR5   = (1<<7) ; 					//使能 USART3 DMA TX
		DMA1_Channel1 -> CCR |= (1<<0) ;
		
		DMA1_U3_Tx_Flag = true;
	}
}

//==============================================================================
//因为数据包太大，为了不影响其他程序的响应速度，将一包发送数据拆成5次载入。
//载入完所有数据后，再使能发送。
//==============================================================================
void BuildUsart3Data(void)
{
	static uint8_t Phase = 0 ; 
	uint8_t i = 0 ; 
	uint16_t DataTemp = 0 ; 
	
	//载入前 17 Byte 数据
	if(Phase == 0)
	{
	  	//                  L                         D                          A                         T                         X                          1                        0
		U3_DMA_TXBuff[0]  = 0x4C ; U3_DMA_TXBuff[1] = 0X44 ; U3_DMA_TXBuff[2] = 0x41 ; U3_DMA_TXBuff[3] = 0x54 ; U3_DMA_TXBuff[4] = 0X58 ; U3_DMA_TXBuff[5] = 0x31 ; U3_DMA_TXBuff[6] = 0x30 ; //起始头(7Byte)
		U3_DMA_TXBuff[7]  = RF_TypeVersion ;											//遥控器类型(FRSKYD16)
		U3_DMA_TXBuff[8]  = 17   ; U3_DMA_TXBuff[9] =  12 ; U3_DMA_TXBuff[10] = 27 ; 						//软件编译时间 : 2017 - 12 - 27
		U3_DMA_TXBuff[11] = MasterInitProtocolVersion ; 									//无线协议版本号 (1Byte)
		U3_DMA_TXBuff[12] = RFHabit ;												//美国手/日本手选择 (0:美国手 / 1:日本手)
		U3_DMA_TXBuff[13] = 0x00 ; //(TransmitterID >> 24)&0xff ; 								//遥控器ID号(SFHSS 只有16Byte 所以高16位固定为0)
		U3_DMA_TXBuff[14] = 0x00 ; //(TransmitterID >> 16)&0xff ;
		U3_DMA_TXBuff[15] = (TransmitterID >> 8)&0xff ;
		U3_DMA_TXBuff[16] = TransmitterID &0xff ; 
	}
	//载入 16通道 数据
	else if(Phase == 4)
	{
	  	for(i = 0 ; i < PTOTOCOL_MAX_CHANNEL ; i++)
		{
			if(i < TRANSMITTER_CHANNEL) DataTemp = FRSKYD16_SendDataBuff[i];
			else DataTemp = 0;
			
			U3_DMA_TXBuff[17 + 2*i] = DataTemp >> 8;
			U3_DMA_TXBuff[18 + 2*i] = DataTemp & 0xFF;
		}
	}
	//载入 通道反向标志位(1Byte) + 所有按键 + 8位拨码开关GPIO值(2Byte) + 电池电压值(2Byte) + 参考电431 高频模块 中位校准成功 标志位
	else if(Phase == 7)
	{
		U3_DMA_TXBuff[49] = ChannelInversion_flg ; 			//通道反向标志位
		
		DataTemp  = 0x0000 ; 
		if(GPIOC -> IDR & (1<<4)) DataTemp |= 0x2000 ; 
		if(GPIOC -> IDR & (1<<5)) DataTemp |= 0x1000 ; 
		
		if(GPIOE -> IDR & (1<<5)) DataTemp |= 0x0800 ; 
		if(GPIOE -> IDR & (1<<4)) DataTemp |= 0x0400 ; 
		
		if(GPIOD -> IDR & (1<<1)) DataTemp |= 0x0200 ; 
		if(GPIOD -> IDR & (1<<0)) DataTemp |= 0x0100 ; 
		if(GPIOC -> IDR & (1<<7)) DataTemp |= 0x0080 ; 
		if(GPIOC -> IDR & (1<<6)) DataTemp |= 0x0040 ; 
		if(GPIOE -> IDR & (1<<7)) DataTemp |= 0x0020 ; 
		
		if(GPIOE -> IDR & (1<<3)) DataTemp |= 0x0010 ; 
		if(GPIOE -> IDR & (1<<2)) DataTemp |= 0x0008 ; 
		if(GPIOE -> IDR & (1<<0)) DataTemp |= 0x0004 ; 
		if(GPIOE -> IDR & (1<<1)) DataTemp |= 0x0002 ; 
		if(GPIOE -> IDR & (1<<6)) DataTemp |= 0x0001 ; 
		U3_DMA_TXBuff[50] = DataTemp >> 8 ;
		U3_DMA_TXBuff[51] = DataTemp & 0xFF ;
		
		
		DataTemp = 0x0000 ; 
		if(GPIOA -> IDR & (1<<4)) DataTemp |= 0x80 ;  // T
		if(GPIOA -> IDR & (1<<5)) DataTemp |= 0x40 ;  // A
		if(GPIOA -> IDR & (1<<6)) DataTemp |= 0x20 ;  // E
		if(GPIOA -> IDR & (1<<7)) DataTemp |= 0x10 ;  // R 
		
		if(GPIOF -> IDR & (1<<0)) DataTemp |= 0x08 ;  //CH5
		if(GPIOF -> IDR & (1<<1)) DataTemp |= 0x04 ;  //CH6
		if(GPIOG -> IDR & (1<<2)) DataTemp |= 0x02 ;  //CH7
		if(GPIOG -> IDR & (1<<3)) DataTemp |= 0x01 ;  //CH8
		U3_DMA_TXBuff[52] = DataTemp&0xFF ;
		
		DataTemp = (uint16_t)(VoltValue * 100) ; 
		U3_DMA_TXBuff[53] = DataTemp >> 8 ;
		U3_DMA_TXBuff[54] = DataTemp & 0xFF ;
		
		DataTemp = 0 ; 
		if(ISCJ431OKFlg)           		DataTemp |= (1<<2) ;
		if(CommunicationError_flg == false) 	DataTemp |= (1<<1) ;
		if(FTDebug_err_flg == false) 	        DataTemp |= (1<<0) ;
		U3_DMA_TXBuff[55] = DataTemp & 0xFF ;
	}
	//载入 所有通道AD采样原始值 + LED状态 + 报警状态 + 定时器死机次数 + 四摇杆偏置值(4Byte) + 四摇杆中位AD值
	else if(Phase == 8)
	{
	  	//7通道AD值
		for(i = 0 ; i < 7 ; i++)
		{
			DataTemp = ADC_Value[i] ; 
			U3_DMA_TXBuff[56 + 2*i] = DataTemp >> 8 ;
			U3_DMA_TXBuff[57 + 2*i] = DataTemp & 0xFF ;
		}
		//LED状态
		U3_DMA_TXBuff[70] = LED_Status_SendDat ; 
		//报警状态
		U3_DMA_TXBuff[71] = RunStatus ; 
		//定时器死机次数
		U3_DMA_TXBuff[72] = TIM2_ErrorCnt ; 
		//四摇杆偏置值 + 4摇杆极值(最大、中位、最小AD值)
		for(i = 0 ; i < 4 ; i++)
		{
			U3_DMA_TXBuff[73+i] = Sampling_Offset[i];
			
			DataTemp = Sampling_MaxMinData[i][MAXDAT];
			U3_DMA_TXBuff[77 + 2*i] = DataTemp >> 8;
			U3_DMA_TXBuff[78 + 2*i] = DataTemp & 0xFF;
			
			DataTemp = Sampling_MaxMinData[i][MIDDAT];
			U3_DMA_TXBuff[85 + 2*i] = DataTemp >> 8;
			U3_DMA_TXBuff[86 + 2*i] = DataTemp & 0xFF;
			
			DataTemp = Sampling_MaxMinData[i][MINDAT];
			U3_DMA_TXBuff[93 + 2*i] = DataTemp >> 8;
			U3_DMA_TXBuff[94 + 2*i] = DataTemp & 0xFF;
		}
	}
	else if(Phase == 9)
	{
	  	//所有数累加和
	  	DataTemp = 0 ; 
		for(i = 0 ; i < (U3_Buffer_Size - 2) ; i++)
		{
			DataTemp += U3_DMA_TXBuff[i] ; 
		}
		U3_DMA_TXBuff[101] = DataTemp >> 8;
		U3_DMA_TXBuff[102] = DataTemp & 0xFF;
	}
	
	if(++Phase > 10) { Phase = 0 ; U3_DMATX_ONOFF() ;} //开始发送
}



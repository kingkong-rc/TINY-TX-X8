/*******************************************************************************
--------------------------------------------------------------------------------
FRSKY-D16 注意的几点 : 
(1) 遥控器按住对码键上电,进入对码状态(持续发送1000个数据包(18mS*1000 = 18S)，退出对码状态)。
(2) 根据遥控器 ID 号 ，生成一个跳频列表(共47个频段)。
--------------------------------------------------------------------------------
FRSKYD16协议 : 整周期 18mS  分两包数据发送， 每包数据 30Byte (接收机32Byte , 附加了1Byte RSSI   1Byte LQI)  前8通发送正常数据 ， 后8通累加 2048 在发送(两包数据在不同频道内接收)
               对码 : 固定在 通道 0 
	       跳频 : 固定 47 通道 ， 通过遥控器生成的随机数来决定跳频步调。

对码数据包(30Byte) :  0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29
                      0x1D 0x03 0x01 0xAA 0xBB 0xCC 0xDD 0xEE 0xFF 0xGG 0xHH 0xII 0xJJ 0X00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xKK 0xLL 
其中 : 
[0] : 0x1D
[1] : 0x03
[2] : 0x01
[3][4] : 遥控器ID号  
[5]   [6][7][8][9][10]: [5] 为通当前道号  [6][7][8][9][10] 为 [5] 开始 后 5(包括[5]) 通道频点列表。
[11] : 为遥控器型号代码(FRSKY_X7 : 0x05    FRSKY_X9D_Plus : 0x03)
[12] : 为遥控器设置的接收序列号( 0 -> 63) 
[13] -> [27] : 固定为0x00
[28][29]: 为数据包校验和(需要查表 [3] -> [27])  


正常数据包(30Byte) :  0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28   29
                      0x1D 0xAA 0xBB 0xCC 0xDD 0xEE 0xFF 0xGG 0x00 0xHH 0xII 0xJJ 0xKK 0xLL 0xMM 0xNN 0xOO 0xPP 0xQQ 0xRR 0xSS 0xTT 0x00 0x00 0x00 0x00 0x00 0x00 0xKK 0xLL 
其中 :
[0]    : 0x1D
[1][2] : 遥控器 ID 号
[3]    : 为遥控器型号代码(FRSKY_X7 : 0x05    FRSKY_X9D_Plus : 0x03)
[4]    : (ctr<<6) + hopping_frequency_no(当前跳频通道号)
[5]    : (chanskip - ctr)>>2 (chanskip为跳频号间隔)
[6]    : 为遥控器设置的接收序列号( 0 -> 63) 
[7]    : 掉线保护设置通道标志位(如果此标志位不为0  表示这包数据内有掉线保护设置数据   此程序直接屏蔽了掉线保护数据 大约会损失 1.6% 数据包，几乎不影响使用)
[9][10][11]  : 通道1、2/9、10的数据 (小于2048为通道1、2   反之为通道9 、10)
[12][13][14] : 通道3、4/11、12的数据(小于2048为通道3、4   反之为通道11、12)
[15][16][17] : 通道5、6/13、14的数据(小于2048为通道5、6   反之为通道13、14)
[18][19][20] : 通道7、8/15、16的数据(小于2048为通道7、8   反之为通道15、16)
[21] : 遥控器上一次发送序列号 + 上一次接收序列号(用于遥测)
[22] -> [27] : 固定为 0x00 
[28][29] : 为数据包校验和(需要查表 [3] -> [27])  

--------------------------------------------------------------------------------
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//某些接收机对时序要求特别严格 : 所以任何影响时序的部分修改特别小心。(精确到 uS)
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

*******************************************************************************/
#include "include.h"

#define FRSKYD16_PACKET_LEN  30
#define FRSKYD16_BINDCHANNEL 47 						//跳频列表第47个频段固定为对码频段 0 



bool     Bind_flg = false ; 
uint16_t FRSKYD16_BindCounts = 0 ; 						// 对码数据包发送个数

uint8_t	 FRSKYD16_ChannelShip = 0 ;       					// 跳频间隔(前后两次频段间隔)
uint8_t  FRSKYD16_ctr = 0 ; 							
uint8_t  FRSKYD16_CountsRst = 0 ; 						
uint8_t  FRSKYD16_HOPChannel[50] ; 						// 跳频列表(根据遥控器ID计算出47个跳频号(后三个频点无效))	
uint8_t  FRSKYD16_calData[50];							// 记录跳频通道频率值

uint8_t  FRSKYD16_Channel_Num = 0   ; 						// 跳频通道号
bool CommunicationError_flg = false ; 
bool HighThrottle_flg = true ; 							//高油门标志位
uint16_t TransmitterID ; 							//遥控器唯一ID
uint8_t  SendPacket[40] ; 							//发送数据包缓存 (1) 对码数据包14Byte   (2)发送遥控数据包 28Byte(8 + 16CH*2 = 40)

typedef enum 
{
	FRSKYD16_BIND  		= 0x00 , 
	FRSKYD16_BIND_PASSBACK	= 0x01 , 
	FRSKYD16_DATA  		= 0x02 ,	
    	FRSKYD16_TUNE  		= 0x03 ,
}FRSKYD16PhaseTypeDef ;
FRSKYD16PhaseTypeDef FRSKYD16Phase = FRSKYD16_DATA ; 

//Channel values are 12-bit values between 988 and 2012, 1500 is the middle.
uint16_t FRSKYD16_SendDataBuff[16]  = { 1500 , 1500 , 988 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500 , 1500};

//FRSKYD16 通道发送顺序
const uint8_t  FRSKYD16_CH_Code[16] = {AILERON, ELEVATOR, THROTTLE, RUDDER, AUX1, AUX2, AUX3, AUX4, AUX5, AUX6, AUX7, AUX8, AUX9, AUX10, AUX11, AUX12};

//==============================================================================
//			FRSKYD16 初始化器件地址
//==============================================================================
static void FRSKYD16_InitDeviceAddr(bool IsBindFlg)
{
	CC2500_WriteReg(CC2500_0C_FSCTRL0 , 0x00); 
	CC2500_WriteReg(CC2500_18_MCSM0,    0x08) ;	
	CC2500_WriteReg(CC2500_09_ADDR , IsBindFlg ? 0x03 : (TransmitterID & 0xFF));
	CC2500_WriteReg(CC2500_07_PKTCTRL1,0x05);
}

//==============================================================================
//			FRSKYD16 设置发送通道
//==============================================================================
static void FRSKYD16_TuneChannel(uint8_t Channel)
{
	CC2500_Strobe(CC2500_SIDLE);						//进入闲置状态
	CC2500_WriteReg(CC2500_25_FSCAL1, FRSKYD16_calData[Channel]);		//设置发送通道
	CC2500_WriteReg(CC2500_0A_CHANNR, FRSKYD16_HOPChannel[Channel]);	//设置发送通道
	CC2500_Strobe(CC2500_SCAL);						//校准频率合成器并关闭
}

//==============================================================================
//				CRC校验
//==============================================================================
static const uint16_t CRC_Short[16] = 	//CRC 校验种子
{
	0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
	0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7 
};

static uint16_t CRCTable(uint8_t val)
{
	uint16_t word ;
	word = CRC_Short[val&0x0F] ;
	val /= 16 ;
	return word ^ (0x1081 * val) ;
}

static uint16_t crc_x(uint8_t *data, uint8_t len)
{
	uint16_t crc = 0;
	for(uint8_t i=0; i < len; i++)
	{
		crc = (crc<<8) ^ CRCTable((uint8_t)(crc>>8) ^ *data++);
	}
	return crc;
}

//通道数据换算
static uint16_t  CalChannelData( uint8_t channel)
{	
  	if(channel > 15) channel = 15 ; 
  	uint16_t DataTemp = FRSKYD16_SendDataBuff[channel] ; 
	
  	if(HighThrottle_flg == true)
	{
		if(channel == THROTTLE) DataTemp =  THR_Output_Min ; 
		else 		        DataTemp =  Output_Mid ; 
	}
	
	return (uint16_t)(((DataTemp - 860) * 3) >> 1) + 64;		//mapped 0 ,2140(125%) range to 64,1984 ;
}


//==============================================================================
//			SFHSS 快速切换发射通道
//解释 : 因为上电时已经校准过频率，且回读了频率值，所以此处可以直接设置前面回读的
//频率值。避免了频率校准时间。
//==============================================================================
static void FRSKYD16_tune_chan_fast(void)
{
	CC2500_Strobe(CC2500_SIDLE);
	CC2500_WriteReg(CC2500_0D_FREQ2 , Fre_Carrier_H);
	CC2500_WriteReg(CC2500_0E_FREQ1 , Fre_Carrier_M);
	CC2500_WriteReg(CC2500_0F_FREQ0 , Fre_Carrier_L);
	DelayUs(2);
	CC2500_Strobe(CC2500_SIDLE);
	CC2500_WriteReg(CC2500_0D_FREQ2 , Fre_Carrier_H);
	CC2500_WriteReg(CC2500_0E_FREQ1 , Fre_Carrier_M);
	CC2500_WriteReg(CC2500_0F_FREQ0 , Fre_Carrier_L);
	CC2500_WriteReg(CC2500_25_FSCAL1, FRSKYD16_calData[FRSKYD16_Channel_Num]);
	CC2500_WriteReg(CC2500_0A_CHANNR, FRSKYD16_HOPChannel[FRSKYD16_Channel_Num]);
	//CC2500_Strobe(CC2500_SCAL);
	//CC2500_Strobe(CC2500_SRX);
}

//==============================================================================
//			计算下一个频道
//==============================================================================
static void FRSKYD16_calc_next_chan(void)
{
    	FRSKYD16_Channel_Num = (FRSKYD16_Channel_Num + FRSKYD16_ChannelShip) % 47 ;
}

//==============================================================================
//对码包格式
//==============================================================================
static void  FRSKYD16_build_Bind_packet(void)
{
  	//固定码
	SendPacket[0] 	= 0x1D;       
	SendPacket[1] 	= 0x03;          
	SendPacket[2] 	= 0x01; 
	
	//遥控器 ID 号	
	SendPacket[3]   = (TransmitterID >> 8) & 0xFF  ;
	SendPacket[4]   = TransmitterID & 0xFF ;

	uint8_t  idx 	= (FRSKYD16_BindCounts % 10) * 5 ;
	SendPacket[5]   = idx;	
	SendPacket[6]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[7]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[8]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[9]   = FRSKYD16_HOPChannel[idx++];
	SendPacket[10]  = FRSKYD16_HOPChannel[idx++];
	SendPacket[11]  = 0x02;
	SendPacket[12]  = 0x01; 
	
	//固定为0
	SendPacket[13] 	= 0x00;
	SendPacket[14] 	= 0x00;
	SendPacket[15] 	= 0x00;
	SendPacket[16] 	= 0x00;
	SendPacket[17] 	= 0x00;
	SendPacket[18] 	= 0x00;
	SendPacket[19] 	= 0x00;
	SendPacket[20] 	= 0x00;
	SendPacket[21] 	= 0x00;
	SendPacket[22] 	= 0x00;
	SendPacket[23] 	= 0x00;
	SendPacket[24] 	= 0x00;
	SendPacket[25] 	= 0x00;
	SendPacket[26] 	= 0x00;
	SendPacket[27] 	= 0x00;
	
	//数据包校验和
	uint16_t lcrc = crc_x(&SendPacket[3], 25);
	
	SendPacket[28] = lcrc >> 8;
	SendPacket[29] = lcrc;
}

//==============================================================================
//数据包格式
//==============================================================================
static void  FRSKYD16_build_Data_packet(void)
{
	static uint8_t lpass;
	uint16_t chan_0 ;
	uint16_t chan_1 ; 
	uint8_t startChan = 0;
	
	// 固定码 + 遥控器 ID
	SendPacket[0] 	= 0x1D; 
	SendPacket[1]   = (TransmitterID >> 8) & 0xFF  ;
	SendPacket[2]   = TransmitterID & 0xFF ;
	SendPacket[3] 	= 0x02;
	
	//  
	SendPacket[4] = (FRSKYD16_ctr<<6) + FRSKYD16_Channel_Num; 
	SendPacket[5] = FRSKYD16_CountsRst;
	SendPacket[6] = 0x01;
	
	
	if(FRSKYD16_Channel_Num == 0x21) 
	  FRSKYD16_Channel_Num = 0x21 ; 
	//
	SendPacket[7] = 0;
	SendPacket[8] = 0;
	
	//
	if (lpass & 1) startChan += 8 ;
	
	//发送通道数据
	for(uint8_t i = 0; i <12 ; i+=3)
	{
	  	//12 bytes
		chan_0 = CalChannelData(startChan);		 
		if(lpass & 1 ) chan_0 += 2048;			
		startChan += 1 ;
		
		//
		chan_1 = CalChannelData(startChan);		
		if(lpass & 1 ) chan_1+= 2048;		
		startChan+=1;
		
		//
		SendPacket[9+i]   = (chan_0 & 0xFF); 
		SendPacket[9+i+1] = (((chan_0>>8) & 0x0F)|(chan_1 << 4));
		SendPacket[9+i+2] = chan_1 >> 4;
	}

	SendPacket[21] = 0x08 ; 
	//下一包数据 发送 后 8 通
	lpass += 1 ;
	
	for (uint8_t i=22;i<28;i++)
	{
		SendPacket[i]=0;
	}
	uint16_t lcrc = crc_x(&SendPacket[3], 25);
	
	SendPacket[28]=lcrc>>8;   //high byte
	SendPacket[29]=lcrc;      //low byte
}

//==============================================================================
//FRSKYD16 : 计算 FRSKYD16 通道(通过计算得到 47 个通道 。轮询时，在这47个通道间跳频)
//相邻两频段间隔在 5 以上
// 1  - 26  : 取 16 个频点
// 27 - 52  : 取 15 个频点
// 53 - 76  : 取 16 个频点
//==============================================================================
void Calc_FRSKYD16_Channel()
{
	uint8_t  idx = 0;
	uint16_t id_tmp = ~ TransmitterID; 					//ID号 按位取反
	
	while(idx < 47)
	{
		uint8_t i;
		uint8_t count_1_26 = 0, count_27_52 = 0, count_53_76 = 0;
		id_tmp = id_tmp * 0x0019660D + 0x3C6EF35F;			// Randomization
		uint8_t next_ch = ((id_tmp >> 8) % 0x4B) + 1;			// Use least-significant byte and must be larger than 1
		for (i = 0; i < idx; i++)
		{
			if(FRSKYD16_HOPChannel[i] == next_ch)    	break;
			if(FRSKYD16_HOPChannel[i] < 27) 		count_1_26++;
			else if (FRSKYD16_HOPChannel[i] < 53)  		count_27_52++;
			else                                    	count_53_76++;
		}
		if (i != idx)  continue;					//说明没有比对完(和其中一个频道重叠，放弃本频道，继续选择)
		
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// 相邻两频道步调要大于 5 以上 (第一个频道不用判断)
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		if(idx)
		{
			uint8_t Temp = 0 ; 
			if(next_ch > FRSKYD16_HOPChannel[idx - 1]) 	Temp = next_ch - FRSKYD16_HOPChannel[idx - 1] ; 
			else 						Temp = FRSKYD16_HOPChannel[idx - 1] - next_ch ; 
		  	if(Temp < 5)	continue ; 
		}
		
		if(next_ch == 0)  break; 
		
		// 1  - 26  : 取 16 个频点
		// 27 - 52  : 取 15 个频点
		// 53 - 76  : 取 16 个频点
		if (((next_ch < 27) && (count_1_26 < 16)) || ((next_ch >= 27) && (next_ch < 53) && (count_27_52 < 15)) || ((next_ch >= 53) && (count_53_76 < 16)))
		{
			FRSKYD16_HOPChannel[idx++] = next_ch;
		}
	}
	
	//对码频段固定为 0 
	FRSKYD16_HOPChannel[FRSKYD16_BINDCHANNEL] = 0 ; 
}

//==============================================================================
//无线发送跳转程序
//==============================================================================
uint16_t ReadFRSKYD16(void)
{
  	static uint8_t Cnts = 0 ; 
	switch(FRSKYD16Phase)
	{
		//发送对码数据包
		case FRSKYD16_BIND : 
		  	if(FRSKYD16_BindCounts < 1200)
			{
				FRSKYD16_TuneChannel(FRSKYD16_BINDCHANNEL) ; 
				CC2500_Strobe(CC2500_SFRX);
		  		FRSKYD16_build_Bind_packet() ;
				CC2500_Strobe(CC2500_SIDLE);
				CC2500_WriteData(SendPacket, FRSKYD16_PACKET_LEN);
				++FRSKYD16_BindCounts ; 
				FRSKYD16Phase = FRSKYD16_BIND_PASSBACK ; 
				Cnts = 0 ; 
			}  
			else
			{
			  	Bind_flg = false ; 
				FRSKYD16_BindCounts = 0 ; 
				FRSKYD16_Channel_Num = 0 ; 
				FRSKYD16_InitDeviceAddr(Bind_flg) ;
				
				//注意 : 只在正常情况下 ， 清零对码指示灯闪烁 ; 如果进入校准后 ， 会在另外一个地方清零
				if(MenuCtrl.RunStep == __stSarttUp)
				{
					LED_State_Shake &= ~LED_BIND  ; 
					LED_State_ON    |= LED_BIND   ; 
					
					if(RunStatus == __stNormal)
					{
						beepCmd(BindFreCounts , __stStop);
					}
				}
				
				FRSKYD16Phase = FRSKYD16_DATA ; 
				
			}
			return 1925 ;
		
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// 用于将对码包间的 9mS间隔拆成 4个 2mS(用于遥控器时基)
		// 实际协议中并不需要此步操作
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		case FRSKYD16_BIND_PASSBACK :
		  	if(Cnts < 3)
			{
				++Cnts ; 
			}
			else 
			{
				FRSKYD16Phase = FRSKYD16_BIND ;
			}
		  	return 2634 ;
		  
		//发送数据包
		case FRSKYD16_DATA : 
		  	FRSKYD16_calc_next_chan();
			FRSKYD16_tune_chan_fast();
			FRSKYD16_build_Data_packet();
			CC2500_Strobe(CC2500_SIDLE);	
			CC2500_WriteData(SendPacket, FRSKYD16_PACKET_LEN);
			FRSKYD16Phase = FRSKYD16_TUNE ;
			Cnts = 0 ; 
			return 1555 ;  
			
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// 用于将数据包间的 9mS间隔拆成 4个 2mS(用于遥控器时基)
		// 实际协议中并不需要此步操作
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		case FRSKYD16_TUNE :
		  	if(Cnts < 3)
			{
				++Cnts ; 
			}
			else 
			{
				FRSKYD16Phase = FRSKYD16_DATA ;
			}
			return 2634 ;
		  
		  
	}
	return 0 ; 
}


void SetBind(void)
{
  	FRSKYD16_BindCounts = 0 ; 
	FRSKYD16Phase = FRSKYD16_BIND ;
	FRSKYD16_TuneChannel(FRSKYD16_HOPChannel[FRSKYD16_BINDCHANNEL]) ; 
}

//==============================================================================
//			FRSKYD16 初始化
//==============================================================================
void initFRSKYD16(void)
{
  	//获取 遥控器 ID 号
  	TransmitterID = GetUniqueID();

	//通过 遥控器 ID 号，计算出 47 个跳频号 
	Calc_FRSKYD16_Channel();
	
	//通过多通道 ADC 获取一个 1 - 46 之间的随机数
	FRSKYD16_ChannelShip = (ADC_Value[0] + ADC_Value[1] + ADC_Value[2] + ADC_Value[3] + ADC_Value[4] + ADC_Value[5] + ADC_Value[6])% 46 + 1 ; 	 // Initialize it to random 0-47 inclusive(取悬空IO ADC)
	while((FRSKYD16_ChannelShip - FRSKYD16_ctr) % 4) FRSKYD16_ctr = (FRSKYD16_ctr + 1) % 4 ;
	FRSKYD16_CountsRst = (FRSKYD16_ChannelShip - FRSKYD16_ctr) >> 2 ; 
	
	//初始化 CC2500 , 返回初始化是否成功标志位
	CommunicationError_flg = CC2500_Init() ; 
	if(CommunicationError_flg == true)
	{
	  	//无线初始化失败，故障报警
	  	if(RunStatus < __stRF_err)				//状态更新前需要判断状态等级，是否更高(否则不更新,不提示)
		{
			beepCmd(NormalFreCounts , __stRFModelLostWarning);
			RunStatus = __stRF_err ; 
			LED_State_ON = LED_NONE ; 
			LED_State_Shake = LED_ALL ; //所有通道LED闪烁
		}
		
		//======================================================
		//跳转到故障报警状态
		//======================================================
		MenuCtrl.RunStep = __stError ;
		MenuCtrl.Sub_RunStep = 0 ; 
	}
	else
	{
		// 校准 各通道频率值
		for (uint8_t i = 0 ; i < 48 ; i++)
		{
			CC2500_Strobe(CC2500_SIDLE);
			CC2500_WriteReg(CC2500_0A_CHANNR , FRSKYD16_HOPChannel[i]);
			CC2500_Strobe(CC2500_SCAL);
			DelayUs(1000);
			FRSKYD16_calData[i]  =  CC2500_ReadReg(CC2500_25_FSCAL1);
		}
		
		
		//按住对码按键上电 进入对码模式，否则
		if(!(GPIOE -> IDR & (1<<7)))
		{
			Bind_flg = true ;
		}
		
		FRSKYD16Phase = FRSKYD16_DATA ;
		FRSKYD16_TuneChannel(FRSKYD16_HOPChannel[FRSKYD16_Channel_Num]) ; 
	}
	
	
	
}
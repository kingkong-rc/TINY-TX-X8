#ifndef _FRSKYD8_H_
#define _FRSKYD8_H_

//定义各通道名称(最大支持8通道)
typedef enum
{
  	RUDDER   	= 0 , 
	THROTTLE 	= 1 , 
	ELEVATOR 	= 2 , 
	AILERON  	= 3 , 
	AUX1  		= 4 , 
	AUX2		= 5 , 
	AUX3		= 6 , 
	AUX4		= 7 , 
}ChannelTypeDef ;


extern bool CommunicationError_flg ;

#define RF_TypeVersion		  0x44						//遥控器类型 'D' -> FRSKYD8
#define MasterInitProtocolVersion 0x01						//无线协议版本号
#define PTOTOCOL_MAX_CHANNEL      8						//协议支持最大发送通道 8 通道
#define TRANSMITTER_CHANNEL       8

extern bool CommunicationError_flg ; 
extern bool HighThrottle_flg ; 
extern bool Bind_flg ; 
extern uint16_t TransmitterID ; 
////////////////////////////////////////////////////////////////////////////////
//SFHSS 8通 无线数据发送数据
////////////////////////////////////////////////////////////////////////////////
extern uint16_t FRSKYD8_SendDataBuff[8];
extern const uint8_t FRSKYD8_CH_Code[8];
void     initFRSKYD8(void);
uint16_t ReadFRSKYD8(void);
void SetBind(void) ; 

#endif


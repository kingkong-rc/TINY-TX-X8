/*
CC2500注意 : 
(1)SPI数据高位在前 , abxx xxxx  其中 : a : 为读写标志位(1 : 读   0 : 写)   b : 突发访问标志位     CS拉低后，必须等待SO引脚拉低，才能开始SPI通信。
*/
#include "include.h"

#define CC25_CSN_on   {GPIOF -> ODR |=  (1<<6);}
#define CC25_CSN_off  {GPIOF -> ODR &= ~(1<<6);}

#define FRSKYD16_CONFIG_CNTS  36
static const uint8_t cc2500_conf[FRSKYD16_CONFIG_CNTS][2]=
{
	{ CC2500_02_IOCFG0   , 0x06 },	// GDO0 指示同步码发送/接收情况
	{ CC2500_00_IOCFG2   , 0x06 },	// GDO2 指示同步码发送/接收情况
	{ CC2500_17_MCSM1    , 0x0c },  // 接收完一个数据包后继续在RX模式 ， 发送完一个数据包后进入 IDLE 模式
	{ CC2500_18_MCSM0    , 0x18 },  // 从 IDLE 进入 TX/RX 模式前，自动校准频率
	{ CC2500_06_PKTLEN   , 0x1E },  // 数据包长度 : 30 Byte
	{ CC2500_07_PKTCTRL1 , 0x04 },  // 接收数据包中时钟包含两 Byte 数据(RSSI 、LQI)
	{ CC2500_08_PKTCTRL0 , 0x01 },  // 数据包长度由同步码第一个字节决定
	{ CC2500_3E_PATABLE  , 0xff },  // PA功率 : 设置PA最大发射功率
	{ CC2500_0B_FSCTRL1  , 0x0A },  // 中值频率 :   Fif = 26M/1024 * 10 = 253906.25Hz
	{ CC2500_0C_FSCTRL0  , 0x00 },	// 频率补偿 :   0
	{ CC2500_0D_FREQ2    , 0x5c },	// 载波频率 :   Fc  = 26M/65536*(0x5C7627) =  24039998474.412109375Hz         
	{ CC2500_0E_FREQ1    , 0x76 },
	{ CC2500_0F_FREQ0    , 0x27 },
	{ CC2500_10_MDMCFG4  , 0x7B },	// 采样带宽 :   BW = 26M / 8*(4+3)*2 = 232142 Hz 
	{ CC2500_11_MDMCFG3  , 0x61 },  // 数据率 :  Rdata = (256 + 0x61)*2048/268435456 * 26M = 70022 Baud   
	{ CC2500_12_MDMCFG2  , 0x13 },  // GFSK  30/32 Bit 同步码
	{ CC2500_13_MDMCFG1  , 0x23 },  // 2Byte 序文   
	{ CC2500_14_MDMCFG0  , 0x7a },  // 频道间隔 : f = 26M/262144 * (256 + 0x7A) * 8 = 299926 Hz
	{ CC2500_15_DEVIATN  , 0x51 },  // MSK 模式有效
	{ CC2500_1B_AGCCTRL2 , 0x03 },	// bind ? 0x43 : 0x03	
	{ CC2500_19_FOCCFG   , 0x16 },  // 频率偏移补偿配置
	{ CC2500_1A_BSCFG    , 0x6c },	// 同步位设置
	{ CC2500_1C_AGCCTRL1 , 0x40 },  // AGC控制  最高位增益关闭  
	{ CC2500_1D_AGCCTRL0 , 0x91 },  
	{ CC2500_21_FREND1   , 0x56 },
	{ CC2500_22_FREND0   , 0x10 },
	{ CC2500_23_FSCAL3   , 0xa9 },
	{ CC2500_24_FSCAL2   , 0x0A },
	{ CC2500_25_FSCAL1   , 0x00 },
	{ CC2500_26_FSCAL0   , 0x11 },
	{ CC2500_29_FSTEST   , 0x59 },
	{ CC2500_2C_TEST2    , 0x88 },
	{ CC2500_2D_TEST1    , 0x31 },
	{ CC2500_2E_TEST0    , 0x0B },
	{ CC2500_03_FIFOTHR  , 0x07 },
	{ CC2500_09_ADDR     , 0x00 }

};

//==============================================================================
//			         延时程序
//==============================================================================
void DelayUs(uint16_t Us)
{
	for( ; Us ; Us--)
	{
		asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
	}
}

void DelayMs(uint16_t Ms)
{
	for( ; Ms ; Ms--)
	{
		DelayUs(1000) ;
	}
}

//==============================================================================
//CS拉低后，必须等待SO拉低，再开始通信
//==============================================================================
static void CC2500_WaitingReady(void)
{
	uint16_t cnts = 0 ; 
	while ((GPIOG->IDR & (1<<7)) && (++cnts < 10000));
}

//==============================================================================
//			    写单个寄存器函数
//==============================================================================
void CC2500_WriteReg(uint8_t address, uint8_t data)
{
	CC25_CSN_off;  		DelayUs(2) ;
	CC2500_WaitingReady();	 
	SPI2_wr_Byte(address); 	DelayUs(2) ;
	SPI2_wr_Byte(data);     DelayUs(2) ;
	CC25_CSN_on;
} 

//==============================================================================
//			   写多个寄存器函数
//==============================================================================
static void CC2500_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t length)
{
	CC25_CSN_off;		DelayUs(2) ;
	CC2500_WaitingReady();
	SPI2_wr_Byte(CC2500_WRITE_BURST | address);
	for(uint8_t i = 0; i < length; i++) SPI2_wr_Byte(data[i]);DelayUs(2) ;
	CC25_CSN_on;
}


//==============================================================================
//			    读单个寄存器函数
//==============================================================================
uint8_t CC2500_ReadReg(uint8_t address)
{ 
	uint8_t result;
	CC25_CSN_off;		DelayUs(2) ;	
	CC2500_WaitingReady();
	SPI2_wr_Byte(CC2500_READ_SINGLE | address);
	result = SPI2_rd_Byte(); 
	CC25_CSN_on;
	return(result); 
} 

//==============================================================================
//			     CC2500命令
//==============================================================================
void CC2500_Strobe(uint8_t state)
{
	CC25_CSN_off;		
	CC2500_WaitingReady();	DelayUs(2) ;
	SPI2_wr_Byte(state); 	DelayUs(2) ;
	CC25_CSN_on;
}

//==============================================================================
//			   写缓存数据
//==============================================================================
void CC2500_WriteData(uint8_t *dpbuffer, uint8_t len)
{
	CC2500_Strobe(CC2500_SFTX);
	CC2500_WriteRegisterMulti(CC2500_3F_TXFIFO, dpbuffer, len);
	CC2500_Strobe(CC2500_STX);
}

//==============================================================================
//		             设置CC2500功率
//==============================================================================
void CC2500_SetPower(uint8_t power)
{
  	static uint8_t prev_power = CC2500_POWER_1 ; 
	if(prev_power != power)
	{	
	  	prev_power = power ; 
		CC2500_WriteReg(CC2500_3E_PATABLE, power);
	}
}

//==============================================================================
//			       CC2500初始化
//==============================================================================
bool  CC2500_Init()
{
  	bool CC2500RestError_flg = false ; 
	SPI2_Init();
	
	//CS_CC2500(PF6)	
	GPIOF -> CR1 |= (1<<6) ; 
	GPIOF -> CR2 |= (1<<6) ; 
	GPIOF -> DDR |= (1<<6) ; 
	CC25_CSN_on;
	
	
	GPIOF -> CR1 |= (1<<5) ; 
	GPIOF -> CR2 |= (1<<5) ; 
	GPIOF -> DDR |= (1<<5) ; 
	
	DelayMs(15);

	//复位 CC2500 
	CC2500_Strobe(CC2500_SRES);
	DelayMs(15);
	
	//读取寄存器值和初始化值比对，判断芯片是否初始化成功
	if(CC2500_ReadReg(CC2500_0E_FREQ1) != 0xC4)  CC2500RestError_flg = true ;
	DelayMs(1);
	DelayMs(10);
	//如果复位成功 , 初始化CC2500 ; 如果失败，不用初始化，置位复位失败标志位。
	if(!CC2500RestError_flg)
	{
		for (uint8_t i = 0 ; i < FRSKYD16_CONFIG_CNTS ; ++i) 
		{
			CC2500_WriteReg(cc2500_conf[i][0], cc2500_conf[i][1]);		        //初始化 CC2500 寄存器
			DelayUs(20);
		}
		CC2500_Strobe(CC2500_SIDLE);					//进入闲置状态
		DelayUs(20);
		CC2500_SetPower(CC2500_POWER_15);				//初始化 小功率发射 (正常发射后，在设置成最大功率)	
		CC2500_Strobe(CC2500_SIDLE);					//进入闲置状态
		DelayUs(20);
		
		//设置寄存器完成后，SPI读取速度切换到6Mhz
		SPI2 -> CR1 &= ~(1<<6) ; 
		SPI2 -> CR1 &= ~((1<<5)|(1<<4)|(1<<3)) ; 
		DelayMs (10);
		SPI2 -> CR1 |= (1<<6) ; 
		
	}
	return CC2500RestError_flg ; 
}
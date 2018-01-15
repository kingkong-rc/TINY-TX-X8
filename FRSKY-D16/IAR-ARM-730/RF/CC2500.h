#ifndef _CC2500_H_
#define _CC2500_H_

enum 
{
    	CC2500_00_IOCFG2           = 0x00,        // GDO2 output pin configuration
    	CC2500_01_IOCFG1           = 0x01,        // GDO1 output pin configuration
    	CC2500_02_IOCFG0           = 0x02,        // GDO0 output pin configuration
    	CC2500_03_FIFOTHR          = 0x03,        // RX FIFO and TX FIFO thresholds
    	CC2500_04_SYNC1            = 0x04,        // Sync word, high byte
    	CC2500_05_SYNC0            = 0x05,        // Sync word, low byte
    	CC2500_06_PKTLEN           = 0x06,        // Packet length
    	CC2500_07_PKTCTRL1         = 0x07,        // Packet automation control
    	CC2500_08_PKTCTRL0         = 0x08,        // Packet automation control
    	CC2500_09_ADDR             = 0x09,        // Device address
    	CC2500_0A_CHANNR           = 0x0A,        // Channel number
    	CC2500_0B_FSCTRL1          = 0x0B,        // Frequency synthesizer control
    	CC2500_0C_FSCTRL0          = 0x0C,        // Frequency synthesizer control
    	CC2500_0D_FREQ2            = 0x0D,        // Frequency control word, high byte
    	CC2500_0E_FREQ1            = 0x0E,        // Frequency control word, middle byte
    	CC2500_0F_FREQ0            = 0x0F,        // Frequency control word, low byte
    	CC2500_10_MDMCFG4          = 0x10,        // Modem configuration
    	CC2500_11_MDMCFG3          = 0x11,        // Modem configuration
    	CC2500_12_MDMCFG2          = 0x12,        // Modem configuration
    	CC2500_13_MDMCFG1          = 0x13,        // Modem configuration
    	CC2500_14_MDMCFG0          = 0x14,        // Modem configuration
    	CC2500_15_DEVIATN          = 0x15,        // Modem deviation setting
    	CC2500_16_MCSM2            = 0x16,        // Main Radio Cntrl State Machine config
    	CC2500_17_MCSM1            = 0x17,        // Main Radio Cntrl State Machine config
    	CC2500_18_MCSM0            = 0x18,        // Main Radio Cntrl State Machine config
    	CC2500_19_FOCCFG           = 0x19,        // Frequency Offset Compensation config
    	CC2500_1A_BSCFG            = 0x1A,        // Bit Synchronization configuration
    	CC2500_1B_AGCCTRL2         = 0x1B,        // AGC control
    	CC2500_1C_AGCCTRL1         = 0x1C,        // AGC control
    	CC2500_1D_AGCCTRL0         = 0x1D,        // AGC control
    	CC2500_1E_WOREVT1          = 0x1E,        // High byte Event 0 timeout
    	CC2500_1F_WOREVT0          = 0x1F,        // Low byte Event 0 timeout
    	CC2500_20_WORCTRL          = 0x20,        // Wake On Radio control
    	CC2500_21_FREND1           = 0x21,        // Front end RX configuration
    	CC2500_22_FREND0           = 0x22,        // Front end TX configuration
    	CC2500_23_FSCAL3           = 0x23,        // Frequency synthesizer calibration
   	CC2500_24_FSCAL2           = 0x24,        // Frequency synthesizer calibration
    	CC2500_25_FSCAL1           = 0x25,        // Frequency synthesizer calibration
    	CC2500_26_FSCAL0           = 0x26,        // Frequency synthesizer calibration
    	CC2500_27_RCCTRL1          = 0x27,        // RC oscillator configuration
    	CC2500_28_RCCTRL0          = 0x28,        // RC oscillator configuration
    	CC2500_29_FSTEST           = 0x29,        // Frequency synthesizer cal control
    	CC2500_2A_PTEST            = 0x2A,        // Production test
    	CC2500_2B_AGCTEST          = 0x2B,        // AGC test
    	CC2500_2C_TEST2            = 0x2C,        // Various test settings
    	CC2500_2D_TEST1            = 0x2D,        // Various test settings
    	CC2500_2E_TEST0            = 0x2E,        // Various test settings

// Status registers
    	CC2500_30_PARTNUM          = 0x30,        // Part number
    	CC2500_31_VERSION          = 0x31,        // Current version number
    	CC2500_32_FREQEST          = 0x32,        // Frequency offset estimate
    	CC2500_33_LQI              = 0x33,        // Demodulator estimate for link quality
    	CC2500_34_RSSI             = 0x34,        // Received signal strength indication
    	CC2500_35_MARCSTATE        = 0x35,        // Control state machine state
   	CC2500_36_WORTIME1         = 0x36,        // High byte of WOR timer
    	CC2500_37_WORTIME0         = 0x37,        // Low byte of WOR timer
    	CC2500_38_PKTSTATUS        = 0x38,        // Current GDOx status and packet status
    	CC2500_39_VCO_VC_DAC       = 0x39,        // Current setting from PLL cal module
    	CC2500_3A_TXBYTES          = 0x3A,        // Underflow and # of bytes in TXFIFO
    	CC2500_3B_RXBYTES          = 0x3B,        // Overflow and # of bytes in RXFIFO

// Multi byte memory locations
    	CC2500_3E_PATABLE          = 0x3E,
    	CC2500_3F_TXFIFO           = 0x3F,
    	CC2500_3F_RXFIFO           = 0x3F,
};

enum CC2500_POWER
{
	CC2500_POWER_0  = 0x00,	// –55dbm or less
	CC2500_POWER_1  = 0x50,	// -30dbm
	CC2500_POWER_2  = 0x44, // –28dbm
	CC2500_POWER_3  = 0xC0, // –26dbm
	CC2500_POWER_4  = 0x84, // –24dbm
	CC2500_POWER_5  = 0x81, // –22dbm
	CC2500_POWER_6  = 0x46, // –20dbm
	CC2500_POWER_7  = 0x93, // –18dbm
	CC2500_POWER_8  = 0x55, // –16dbm
	CC2500_POWER_9  = 0x8D, // –14dbm
	CC2500_POWER_10 = 0xC6,	// -12dbm
	CC2500_POWER_11 = 0x97,	// -10dbm
	CC2500_POWER_12 = 0x6E,	//  -8dbm
	CC2500_POWER_13 = 0x7F,	//  -6dbm
	CC2500_POWER_14 = 0xA9,	//  -4dbm
	CC2500_POWER_15 = 0xBB,	//  -2dbm
	CC2500_POWER_16 = 0xFE,	//   0dbm
	CC2500_POWER_17 = 0xFF	//  +1dbm
};

//初始化载波频率(起始频率)   
#define Fre_Carrier_H  0x5c
#define Fre_Carrier_M  0x76
#define Fre_Carrier_L  0x27

// Definitions for burst/single access to registers
#define CC2500_WRITE_SINGLE     0x00
#define CC2500_WRITE_BURST      0x40
#define CC2500_READ_SINGLE      0x80
#define CC2500_READ_BURST       0xC0

// Strobe commands
#define CC2500_SRES             0x30        // Reset chip.
#define CC2500_SFSTXON          0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).  	如果 MCSM0.FS_AUTOCAL = 1 ， 启用和校准频率合成器。
                                            // If in RX: Go to a wait state where only the synthesizer is           	如果在RX ， 进入只有合成器运行的等待状态(快速RX/TX切换)
                                            // running (for quick RX / TX turnaround).
#define CC2500_SXOFF            0x32        // Turn off crystal oscillator.					    	关闭晶体振荡器
#define CC2500_SCAL             0x33        // Calibrate frequency synthesizer and turn it off				校准频率合成器并将其关闭。SCAL可以从空闲模式中选通，设置手动校准模式
                                            // (enables quick start).
#define CC2500_SRX              0x34        // Enable RX. Perform calibration first if coming from IDLE and		启动RX ， 如果来自IDLE 和 MCSM0.FS_AUTOCAL = 1 先进行校准
                                            // MCSM0.FS_AUTOCAL=1.
#define CC2500_STX              0x35        // In IDLE state: Enable TX. Perform calibration first if			在闲置状态 : 启用TX 。如果 MCSM0.FS_AUTOCAL = 1 ,则先执行校准。如果处于RX状态 并且CCA被使能，仅当通道清零时才转到TX完成
                                            // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:	
                                            // Only go to TX if channel is clear.
#define CC2500_SIDLE            0x36        // Exit RX / TX, turn off frequency synthesizer and exit			退出RX/TX，关闭频率合成器，如果适用的话退出 无线电唤醒
                                            // Wake-On-Radio mode if applicable.
#define CC2500_SAFC             0x37        // Perform AFC adjustment of the frequency synthesizer			
#define CC2500_SWOR             0x38        // Start automatic RX polling sequence (Wake-on-Radio)			启动RX自动轮询序列
#define CC2500_SPWD             0x39        // Enter power down mode when CSn goes high.				当CS拉高，进入电源关闭模式
#define CC2500_SFRX             0x3A        // Flush the RX FIFO buffer.						刷新RX缓冲区。只在闲置或RX FIFO缓存溢出状态下发出SFRX。
#define CC2500_SFTX             0x3B        // Flush the TX FIFO buffer.						刷新TX缓冲区。只在闲置或TX FIFO缓存溢出状态下发出SFRX
#define CC2500_SWORRST          0x3C        // Reset real time clock.							重新设置实时时钟，
#define CC2500_SNOP             0x3D        // No operation. May be used to pad strobe commands to two
                                            // bytes for simpler software.
//----------------------------------------------------------------------------------
// Chip Status Byte
//----------------------------------------------------------------------------------
// Bit fields in the chip status byte
//#define CC2500_STATUS_CHIP_RDYn_BM             0x80
//#define CC2500_STATUS_STATE_BM                 0x70
//#define CC2500_STATUS_FIFO_BYTES_AVAILABLE_BM  0x0F

// Chip states
//#define CC2500_STATE_IDLE                      0x00
//#define CC2500_STATE_RX                        0x10
//#define CC2500_STATE_TX                        0x20
//#define CC2500_STATE_FSTXON                    0x30
//#define CC2500_STATE_CALIBRATE                 0x40
//#define CC2500_STATE_SETTLING                  0x50
//#define CC2500_STATE_RX_OVERFLOW               0x60
//#define CC2500_STATE_TX_UNDERFLOW              0x70


void DelayUs(uint16_t Us);
void DelayMs(uint16_t Ms);
bool CC2500_Init(void);
void CC2500_SetPower(uint8_t power);
void CC2500_Strobe(uint8_t state);
void CC2500_WriteReg(uint8_t address, uint8_t data);
uint8_t CC2500_ReadReg(uint8_t address);
void CC2500_SetTxRxMode(uint8_t mode);
void CC2500_WriteData(uint8_t *dpbuffer, uint8_t len);

#endif



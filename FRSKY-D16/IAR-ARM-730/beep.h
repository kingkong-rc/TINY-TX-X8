#ifndef _BEEP_H_
#define _BEEP_H_

#define NormalFreCounts      1000
#define PowerOnFreCounts     1650
#define BindFreCounts        512

typedef enum
{
  	__stStop			= 0 , 
	
	__stKeyClick 			= 1 , 					//按键提示音
	__stMidOffSet  			= 2 , 					//微调中值点
	__stExitStaticOverTime		= 3 , 					//退出静止超时状态
	__stMidADAdjustDown		= 4 , 					//中位校准完成
	
	__stBindWarning 		= 5 , 					//对码提示音
	__stStaticOverTimeWarning	= 6 , 					//超时报警
	__stRFModelLostWarning   	= 7 , 					//无线模块掉线报警
	__stThrottleHighWarning		= 8 , 					//油门摇杆高上电报警
	__stFastContinumWarning 	= 9 , 					//快速连续报警(包括无校准数据 ， 低电量)
}BeepModeTypeDef ;

void beep_Init(void);
void beepCmd(uint16_t BeepFreCounts , BeepModeTypeDef Mode);
void beepScan(void);

#endif




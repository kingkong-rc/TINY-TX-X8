#ifndef  __STARTUP_H_
#define  __STARTUP_H_

#define MAXDAT 0 
#define MIDDAT 1
#define MINDAT 2 

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//极值校准时，最大值、最小值有效值、以及中位值的有效范围
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define MAXValue_Min  3095
#define MINValue_Max  1000


//2017年12月6日荣改，放宽AD中位界限（从之前的2047±200放宽到2047±350）
//#define AD_MidValue_Min 1847
//#define AD_MidValue_Max 2247
#define AD_MidValue_Min (2047-350)
#define AD_MidValue_Max (2047+350)


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 发送最大值 必须大于 发送最小值
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define Output_Max 2012 
#define Output_Mid 1500
#define Output_Min 988

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//油门最大、最小发送值
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define THR_Output_Max 1995 
#define THR_Output_Mid 1500
#define THR_Output_Min 1005

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// 采样最大值 必须大于 采样最小值
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#define Input_Max  4094   //为了使中位值是个整数(4095中位值是小数)
#define Input_Mid  2047
#define Input_Min  0

//电调起转点(实际上是988+212 = 1200)
#define ESC_StartUp_Value 212

extern uint16_t Sampling_Data[12];
extern uint16_t Sampling_MaxMinData[4][3] ; 
extern uint8_t  Sampling_Offset [4] ; 
extern uint8_t  ChannelInversion_flg ; 

void StartUp(void);
uint16_t Get_SendValue(ChannelTypeDef Channel);
void Init_ChannelDis(bool ONOFF_flg);
void SetOffSet(void);
#endif

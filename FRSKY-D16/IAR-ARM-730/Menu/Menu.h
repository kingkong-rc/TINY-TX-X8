#ifndef _MENU_H
#define _MENU_H_

//遥控器运行状态
//注意状态等级 : 高级状态可以覆盖低级状态，反之无效。
typedef enum
{
  	__stNormal	= 0 , 
	__stStaTimeOver = 1 ,	//启动后,静止超过15Min,超时报警
	__stTHR_err	= 2 , 	//上电时,油门处于高位(计算后发送值超过140 960+140 = 1100)
	__stVolt_err 	= 3 , 	//低电量报警
	__stNOAdjust	= 4 , 	//没有进行中位校准
	__stRF_err      = 5 ,	//无线模块故障
}RunStatusTypeDef;
extern RunStatusTypeDef RunStatus ; 

typedef enum
{
	__stPownOn	= 0 , 
	__stSarttUp	= 1 ,
	__stFTDebug	= 2 , 
	__stError	= 3 , 
}RunStepTypeDef ;

typedef enum 
{
	__AmericaPlayer = 0 , 
	__JapanPlayer   = 1 , 
}RFHabitTypeDef ; 
extern RFHabitTypeDef RFHabit ; 

typedef struct
{	
	RunStepTypeDef RunStep ; 
	uint8_t Sub_RunStep ; 
}MenuCtrlTypeDef;
extern MenuCtrlTypeDef MenuCtrl ; 


void menu(void);

#endif

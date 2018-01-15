#include "include.h"

RFHabitTypeDef RFHabit = __AmericaPlayer ; 

MenuCtrlTypeDef  MenuCtrl = 
{
	.RunStep = __stPownOn , 
	.Sub_RunStep = 0 ,
};
RunStatusTypeDef RunStatus = __stNormal ; 					//运行状态(正常/静止超时报警/油门摇杆高上电报警/CYRF6936掉线报警/低电压报警)


//==============================================================================
//产生严重故障后，停在这里不往下执行(CYRF6936故障 / 未进行中位校准(中位校准数据有误))
//==============================================================================
static void Error(void)
{
  	MenuCtrl.RunStep	= __stError ; 
	MenuCtrl.Sub_RunStep 	=  0 ; 
}

static void (*pMENU[])(void) = 
{
	PowerOn , 								// 0   上电检测(等待检测电压，油门摇杆位置 ，以及硬件是否正常)
	StartUp , 								// 1   遥控器正常工作
	FTDebug , 								// 2   工厂调试模式
	Error , 								// 3   故障(停在这里不往下执行)
};

void menu(void)
{
  	static uint8_t Menu_cnt = 0 ; 
	
	if(Menu_cnt < 5) ++Menu_cnt ; 
	else Menu_cnt = 0 ; 
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//特别注意 : 为了使每次中断响应时间一致，应保证每个片段执行时间，必须小于
	//两中断间隔 , 而且此程序中断间隔不一致。因此要也别小心！！！
	//中断中的 SFHSS_CAL 执行次数也会影响 间隔顺序。
	//0 : 约 4.5mS        1 : 约 : 287us(此处不要执行函数)    2 : 约 : 1.5mS
	//3 : 约 4.5mS        4 : 约 : 287us(此处不要执行函数)    5 : 约 : 1.5mS
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if(Menu_cnt == 0)      { StartupAgain_ADC(); beepScan() ; }		//开始一次AD转换 以及 蜂鸣器驱动				 
	else if(Menu_cnt == 2) { LED_Usart1Send()  ; key_Scan() ; }  		//状态指示灯显示 以及 按键扫描 	
	else if(Menu_cnt == 3)   GetADValue() ; 
	else if(Menu_cnt == 5) { BuildUsart3Data() ; Volt_LED(VoltValue) ;} 	//蜂鸣器 以及 串口 3 发送数据
	
	pMENU[MenuCtrl.RunStep]();
	ChannelKeyValue = __stEmpty_Key ; 
	OffSetKeyValue  = __stOffset_EmptyKey ;
}



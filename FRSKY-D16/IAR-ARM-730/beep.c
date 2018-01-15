/*==============================================================================
Beep 驱动IO TIM1_CH2
蜂鸣器提示种类 :
(1)按键提示音 : 频率:1000Hz  模式 : 短叫/长叫/两次短叫/三次短叫
(2)微调提示音 : 1000(最弱) -> 1600(最强) -> 1000(最弱)  每步调 20HZ * 20 * 2
(3)报警提示音 : (a)遥控器静止超时   		(连续 叫一声)
		(b)摇杆高位报警	    		(连续 叫四声)
		(c)低电量/中位校准报警          (连续 快速叫)
  		(d)CYRF6936掉线报警 		(连续 叫三声)
==============================================================================*/
#include "include.h"

#define ENABLE_PRO_CYLCE	500 					    	//允许可编程周期  5S

static uint16_t Beep_ON_cnt = 0 ; 						//蜂鸣器计数
static BeepModeTypeDef BeepMode = __stStop ; 					//蜂鸣器模式

//==============================================================================
//初始化 BBEP 控制 IO  PD4
//通过 TIM1_CH2 输出频率变化 控制 BEEP 音调
//==============================================================================
void beep_Init(void)
{
  	//打开外设TIM1时钟
	CLK -> PCKENR2 |= (1<<1);
	
	//初始化蜂鸣器 PD4 (TIM1_CH2)
	GPIOD -> CR1 |= (1<<4);
	GPIOD -> CR2 |= (1<<4);
	GPIOD -> DDR |= (1<<4);
	
	//注意: TIM1的频率是 F = Fsc/(PSCR)=12M/(11+1) = 1M
	TIM1 -> PSCRH = 0x00 ;   						
	TIM1 -> PSCRL = 0x0B ;
	
	//蜂鸣器频率 : 初始化 1K
	TIM1 -> ARRH  = (1000000/1000) >> 8 ;					
	TIM1 -> ARRL  = (1000000/1000) & 0xff ;
	
	//比较寄存器始终控制为频率的一半
	TIM1 -> CCR2H = (1000000/2000) >> 8 ;
	TIM1 -> CCR2L = (1000000/2000) & 0xff ;
	
	//PWM1输出模式(向上计数 : CNT < CCR1 输出1  CNT >= CCR1 输出0)
	TIM1 -> CCMR2 = 0x68 ;	
	//使能TIM1_CH2输出  
	TIM1 -> CCER1 = 0x30 ; 							
	//向上计数
	TIM1 -> CR1  &= ~(1<<0);    						//先暂停计数
	TIM1 -> BKR  &= ~(1<<7);						//禁止输出	
	
	BeepMode = __stStop ; 
	Beep_ON_cnt = 0 ; 
}

void beepCmd(uint16_t BeepFreCounts , BeepModeTypeDef Mode)
{
	//防止重复设置
	if(BeepMode != Mode)
	{
	  	TIM1 -> CR1  &= ~(1<<0);    					
		TIM1 -> BKR  &= ~(1<<7);					//关闭输出
	
		//蜂鸣器频率范围 : 1K -> 2K
  		if(BeepFreCounts > 2000)      BeepFreCounts = 2000;
		else if(BeepFreCounts < 1000) BeepFreCounts = 1000 ; 
		
		//清零计数寄存器
		TIM1 -> CNTRH = 0 ; 
		TIM1 -> CNTRL = 0 ; 
		
  		//设置蜂鸣器频率
		TIM1 -> ARRH  = (1000000/BeepFreCounts) >> 8 ;			//蜂鸣器频率 
		TIM1 -> ARRL  = (1000000/BeepFreCounts) & 0xff ;
  
  		TIM1 -> CCR2H = (1000000/(BeepFreCounts*2)) >> 8 ;
		TIM1 -> CCR2L = (1000000/(BeepFreCounts*2)) & 0xFF ;
	
		TIM1 -> CR1  |= (1<<0);
		TIM1 -> BKR  |= (1<<7);
	
  		BeepMode = Mode ;
		Beep_ON_cnt = 0 ; 
	}
}



void beepScan(void)
{
	if(BeepMode == __stStop) 
	{
	  	Beep_ON_cnt = 0 ; 
		TIM1 -> BKR  &= ~(1<<7);
		TIM1 -> CR1  &= ~(1<<0); 
	}
	
	//按键提示音(短叫一声0.025S)
	else if(BeepMode == __stKeyClick)
	{
		if(Beep_ON_cnt < 3) ++Beep_ON_cnt ; 
		else
		{
		  	BeepMode = __stStop ; 
		}
	}
	
	//调整位置 微调数值 经过 中值(长叫一声0.2S)
	else if(BeepMode == __stMidOffSet)
	{
		if(Beep_ON_cnt < 20) ++Beep_ON_cnt ; 
		else
		{
		  	BeepMode = __stStop ; 
		}
	}
	
	//退出静止超时报警(连叫两声)
	else if(BeepMode == __stExitStaticOverTime)
	{
		if(Beep_ON_cnt < 15) ++Beep_ON_cnt ; 
		else
		{
		  	BeepMode = __stStop ; 
		}
		
		if((Beep_ON_cnt < 5) || (Beep_ON_cnt > 10))
		{
		  	if(!(TIM1->BKR & (1<<7))) TIM1 -> BKR |=  (1<<7) ;
		}
		else
		{
			if(TIM1->BKR & (1<<7))    TIM1 -> BKR &= ~(1<<7) ;
		}
	}
	
	//中位校准完成 提示音(长叫1声)
	else if(BeepMode == __stMidADAdjustDown)
	{
		if(Beep_ON_cnt < 40) ++Beep_ON_cnt ; 
		else
		{
		  	BeepMode = __stStop ; 
		}
		
		if(!(TIM1->BKR & (1<<7))) TIM1 -> BKR |=  (1<<7) ;
	}
	
	//======================================================================
	//			报警提示音
	//======================================================================
	//对码提示音
	else if(BeepMode == __stBindWarning)
	{
		if(Beep_ON_cnt < ENABLE_PRO_CYLCE)
		{
		  	++Beep_ON_cnt ; 
			//频率 : 512
			if(((Beep_ON_cnt < 90) && (Beep_ON_cnt >= 40)) || ((Beep_ON_cnt < 150) && (Beep_ON_cnt >= 100)) || ((Beep_ON_cnt < 210) && (Beep_ON_cnt >= 160)) || ((Beep_ON_cnt < 330) && (Beep_ON_cnt >= 280)) || ((Beep_ON_cnt < 500) && (Beep_ON_cnt >= 400)))
			{
			  	if((TIM1 -> ARRH != ((1000000/512) >> 8)) || (TIM1 -> ARRL  != ((1000000/512) & 0xff)) || (!(TIM1 -> BKR & (1<<7))))
				{
					TIM1 -> ARRH  = (1000000/512) >> 8 ;			//蜂鸣器频率 
					TIM1 -> ARRL  = (1000000/512) & 0xff ;
  
  					TIM1 -> CCR2H = (1000000/(512*2)) >> 8 ;
					TIM1 -> CCR2L = (1000000/(512*2)) & 0xFF ;	
					
					TIM1 -> BKR |=  (1<<7) ;
				}
			}
			
			//频率 : 413
			else if(((Beep_ON_cnt < 255) && (Beep_ON_cnt >= 220)) || ((Beep_ON_cnt < 375) && (Beep_ON_cnt >= 340)))
			{
				if((TIM1 -> ARRH != ((1000000/413) >> 8)) || (TIM1 -> ARRL  != ((1000000/413) & 0xff)) || (!(TIM1 -> BKR & (1<<7))))
				{
					TIM1 -> ARRH  = (1000000/413) >> 8 ;			//蜂鸣器频率 
					TIM1 -> ARRL  = (1000000/413) & 0xff ;
  
  					TIM1 -> CCR2H = (1000000/(413*2)) >> 8 ;
					TIM1 -> CCR2L = (1000000/(413*2)) & 0xFF ;	
					
					TIM1 -> BKR |=  (1<<7) ;
				}
			}
			
			//频率 : 610
			else if(((Beep_ON_cnt < 280) && (Beep_ON_cnt >= 255)) || ((Beep_ON_cnt < 400) && (Beep_ON_cnt >= 375)))
			{
				if((TIM1 -> ARRH != ((1000000/610) >> 8)) || (TIM1 -> ARRL  != ((1000000/610) & 0xff)) || (!(TIM1 -> BKR & (1<<7))))
				{
					TIM1 -> ARRH  = (1000000/610) >> 8 ;			//蜂鸣器频率 
					TIM1 -> ARRL  = (1000000/610) & 0xff ;
  
  					TIM1 -> CCR2H = (1000000/(610*2)) >> 8 ;
					TIM1 -> CCR2L = (1000000/(610*2)) & 0xFF ;	
					
					TIM1 -> BKR |=  (1<<7) ;
				}
			}
			
			//其他情况下 熄灭
			else
			{
				if(TIM1->BKR & (1<<7))    TIM1 -> BKR &= ~(1<<7) ;
			}
		}
		else
		{
		  	Beep_ON_cnt = 0 ;  
		}
	}
	
	//遥控器静止超时 : 连续叫一声
	else if(BeepMode == __stStaticOverTimeWarning)
	{
		if(Beep_ON_cnt < 100) ++Beep_ON_cnt;
		else Beep_ON_cnt = 0 ; 
		
		if(Beep_ON_cnt < 5)
		{
		  	if(!(TIM1->BKR & (1<<7))) TIM1 -> BKR |=  (1<<7) ;
		}
		else
		{
			if(TIM1->BKR & (1<<7))    TIM1 -> BKR &= ~(1<<7) ;
		}
	}
	//CYRF6936掉线报警 : 连续叫三声
	else if(BeepMode == __stRFModelLostWarning)
	{
		if(Beep_ON_cnt < 100) ++Beep_ON_cnt;
		else Beep_ON_cnt = 0 ; 
		
		if((Beep_ON_cnt < 5) || ((Beep_ON_cnt > 10)&&(Beep_ON_cnt < 15)) || ((Beep_ON_cnt > 20)&&(Beep_ON_cnt < 25)))
		{
		  	if(!(TIM1->BKR & (1<<7))) TIM1 -> BKR |=  (1<<7) ;
		}
		else
		{
			if(TIM1->BKR & (1<<7))    TIM1 -> BKR &= ~(1<<7) ;
		}
	}
	//油门高上电报警 : 连续叫四声
	else if(BeepMode == __stThrottleHighWarning)
	{
		if(Beep_ON_cnt < 100) ++Beep_ON_cnt;
		else Beep_ON_cnt = 0 ; 
		
		if((Beep_ON_cnt < 5) || ((Beep_ON_cnt > 10)&&(Beep_ON_cnt < 15)) || ((Beep_ON_cnt > 20)&&(Beep_ON_cnt < 25)) || ((Beep_ON_cnt > 30)&&(Beep_ON_cnt < 35)))
		{
		  	if(!(TIM1->BKR & (1<<7))) TIM1 -> BKR |=  (1<<7) ;
		}
		else
		{
			if(TIM1->BKR & (1<<7))    TIM1 -> BKR &= ~(1<<7) ;
		}
	}
	//快速连续叫 : 包括低电量、无中位校准数据
	else if(BeepMode == __stFastContinumWarning)
	{
		if(Beep_ON_cnt < 10) ++Beep_ON_cnt;
		else Beep_ON_cnt = 0 ; 
		
		if(Beep_ON_cnt < 5) 
		{
		  	if(!(TIM1->BKR & (1<<7))) TIM1 -> BKR |=  (1<<7) ;
		}
		else
		{
			if(TIM1->BKR & (1<<7))    TIM1 -> BKR &= ~(1<<7) ;
		}
	}
}
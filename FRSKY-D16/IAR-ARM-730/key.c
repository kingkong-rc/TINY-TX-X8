/*==============================================================================
按键包括 : 
(1) 4*2微调按键(四个摇杆分别有两个微调按键 : 加/减   按键状态包括 : 短按，连发)
(2) CH5(上/下) CH6(上/下)
(3) BIND(对码)   Auto(和 CH6Down 配合 中位校准)
(4) 8个拨码开关
==============================================================================*/
#include "include.h"

//通道切换按键 : 短按
volatile ChannelKeyTypeDef ChannelKeyValue 	= __stEmpty_Key; 

//微调按键 : 短按 / 连发
//短按判定 : down(20mS) -> up(20mS)
//连发判定 : down(1S)   -> down(50mS 触发一次按键) -> up(20mS)
volatile OffSetKeyTypeDef  OffSetKeyValue  	 = __stOffset_EmptyKey ; 
// [x][0] : downcnt        [x][1] : upcnt
static uint8_t ChannelkeyTemp[5][2] = 
{
  	{ 0 , 0 } , //为了与按键顺序对齐
	
	//CH5
	{ 0 , 0 } , //上
	{ 0 , 0 } , //下
	
	//CH6
	{ 0 , 0 } , //上
	{ 0 , 0 } , //下
};

// [x][0] : downcnt 
// [x][1] : continumcnt
// [x][2] : upcnt
static uint8_t OffSetKeyTemp[9][3] = 
{
  	{ 0 , 0 , 0} , //为了与按键顺序对齐
	
  	//R_Plus
  	{ 0 , 0 , 0} ,
  	//R_Sub
  	{ 0 , 0 , 0} ,
	
  	//T_Plus
  	{ 0 , 0 , 0} ,
  	//T_Sub
  	{ 0 , 0 , 0} ,
	
  	//A_Plus
  	{ 0 , 0 , 0} ,
  	//A_Sub
  	{ 0 , 0 , 0} ,
	
  	//E_Plus
  	{ 0 , 0 , 0} ,
  	//E_Sub
  	{ 0 , 0 , 0} ,
};


void key_int(void)
{
	//14个按键(8个微调按键 + 4个 通道按键 + Bind + Auto)
 	//CH5_UP + CH5_DOWN + TRIM_R_UP + TRIM_R_DOWN 
	GPIOC -> CR1 |=  (1<<7)|(1<<6)|(1<<5)|(1<<4)  ; 			
	GPIOC -> CR2 &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4)) ; 
	GPIOC -> DDR &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4)) ;
	
	//TRIM_T_UP + TRIM_T_DOWN
	GPIOD -> CR1 |=  (1<<1)|(1<<0) ; 
	GPIOD -> CR2 &= ~((1<<1)|(1<<0)) ; 
	GPIOD -> DDR &= ~((1<<1)|(1<<0)) ;
	
	//TRIM_A_UP + TRIM_A_DOWN + TRIM_E_UP + TRIM_E_DOWN + CH6_DOWN + CH6_UP + KEY_AUTO + KEY_BIND
	GPIOE -> CR1 |=  (1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0) ; 
	GPIOE -> CR2 &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0)) ; 
	GPIOE -> DDR &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<3)|(1<<2)|(1<<1)|(1<<0)) ;
	
	
	//8个 拨码开关(上拉输入) : 前6个拨码开关对应前6通的反向 ， 后两个为 CH7 / CH8开关通道
	//INV_CH7 + INV_CH8 
  	GPIOG -> CR1 |=  (1<<3)|(1<<2)  ; 
	GPIOG -> CR2 &= ~((1<<3)|(1<<2)) ; 
	GPIOG -> DDR &= ~((1<<3)|(1<<2)) ; 
	
	//INV_CH5 + INV_CH6
  	GPIOF -> CR1 |=  (1<<1)|(1<<0)  ; 
	GPIOF -> CR2 &= ~((1<<1)|(1<<0)) ; 
	GPIOF -> DDR &= ~((1<<1)|(1<<0)) ; 
		
	//INV_A + INV_E + INV_T + INV_R
  	GPIOA -> CR1 |=  (1<<7)|(1<<6)|(1<<5)|(1<<4)   ; 
	GPIOA -> CR2 &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4)) ; 
	GPIOA -> DDR &= ~((1<<7)|(1<<6)|(1<<5)|(1<<4)) ; 
	
	//1个 美国手/日本手 选择焊点   浮空(PC0开漏，无上拉，所以设置成浮空输入，注意PCB上增加上拉电阻)
  	GPIOC -> CR1 &= ~(1<<0) ; 
	GPIOC -> CR2 &= ~(1<<0) ; 
	GPIOC -> DDR &= ~(1<<0) ;
}

static void ChannelKey_Scan(GPIO_TypeDef* GPIOx , uint8_t GPIO_Pin , ChannelKeyTypeDef KeyNum)
{
	if(!(GPIOx -> IDR & (uint8_t)GPIO_Pin))
	{
		 if(ChannelkeyTemp[KeyNum][0] < 4) ++ChannelkeyTemp[KeyNum][0];
		 ChannelkeyTemp[KeyNum][1] = 0;
	}
	else
	{
		if(ChannelkeyTemp[KeyNum][1] < 5) ++ChannelkeyTemp[KeyNum][1];
		else
		{
			if(ChannelkeyTemp[KeyNum][0] >= 4)
			{
				ChannelKeyValue = KeyNum ; 
			}
			ChannelkeyTemp[KeyNum][0] = 0 ;
		}
	}
}

static void OffSetKey_Scan(GPIO_TypeDef* GPIOx , uint8_t GPIO_Pin , OffSetKeyTypeDef KeyNum)
{
  	//按键按下
	if(!(GPIOx -> IDR & (uint8_t)GPIO_Pin))
	{
	  	OffSetKeyTemp[KeyNum][2] = 0;
		if(OffSetKeyTemp[KeyNum][0] < 50) 
		{
			++OffSetKeyTemp[KeyNum][0];
			OffSetKeyTemp[KeyNum][1] = 0 ; 
			
			//触发短按
			if(OffSetKeyTemp[KeyNum][0] == 5) 
			{
				OffSetKeyValue = KeyNum ; 
			}
		}
		else
		{
		  	//触发按键连发
		 	if(OffSetKeyTemp[KeyNum][1] < 15) ++OffSetKeyTemp[KeyNum][1];	
			else  
			{
			  	OffSetKeyTemp[KeyNum][1] = 0 ; 
				OffSetKeyValue = KeyNum ;
			} 
		}
		 
	}
	else
	{	//按键松开 50mS
		if(OffSetKeyTemp[KeyNum][2] < 5) ++OffSetKeyTemp[KeyNum][2];
		else
		{
			OffSetKeyTemp[KeyNum][0] = 0 ;
		}
	}
}

void key_Scan(void)
{
	//CH5
	ChannelKey_Scan(GPIOC , GPIO_Pin_4 , __stKey_CH5_Up) ;
	ChannelKey_Scan(GPIOC , GPIO_Pin_5 , __stKey_CH5_Down) ;
	
	//CH6
	ChannelKey_Scan(GPIOE , GPIO_Pin_5 , __stKey_CH6_Up) ;
	ChannelKey_Scan(GPIOE , GPIO_Pin_4 , __stKey_CH6_Down) ;
	
	//8个微调按键
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!! 注意 : 美国手 和 日本手 微调 按键互换 !!!!!!!!!!!!!!!!!!!!!!
	if(RFHabit == __AmericaPlayer)
	{
		OffSetKey_Scan(GPIOC , GPIO_Pin_7 , __stKey_T_Plus) ; 
		OffSetKey_Scan(GPIOC , GPIO_Pin_6 , __stKey_T_Sub) ; 
		
		OffSetKey_Scan(GPIOE , GPIO_Pin_0 , __stKey_E_Plus) ; 
		OffSetKey_Scan(GPIOE , GPIO_Pin_1 , __stKey_E_Sub) ; 
	}
	else
	{
		OffSetKey_Scan(GPIOE , GPIO_Pin_0 , __stKey_T_Plus) ; 
		OffSetKey_Scan(GPIOE , GPIO_Pin_1 , __stKey_T_Sub) ; 
		
		OffSetKey_Scan(GPIOC , GPIO_Pin_7 , __stKey_E_Plus) ; 
		OffSetKey_Scan(GPIOC , GPIO_Pin_6 , __stKey_E_Sub) ; 
	}

	OffSetKey_Scan(GPIOD , GPIO_Pin_1 , __stKey_R_Plus) ; 
	OffSetKey_Scan(GPIOD , GPIO_Pin_0 , __stKey_R_Sub) ; 

  
	OffSetKey_Scan(GPIOE , GPIO_Pin_3 , __stKey_A_Plus) ; 
	OffSetKey_Scan(GPIOE , GPIO_Pin_2 , __stKey_A_Sub) ; 
	

	
	//! 左边微调按键同一时刻只能触发一次，右边微调按键同一时刻只能触发一次
	//!!!!!!!!! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	/*uint8_t LefOffSetKey_Temp = 0xFF ;   uint8_t RightOffSetKey_Temp = 0xFF ; 
	static uint8_t LefOffSetKey_LastTemp   = 0xFF ; 
	static uint8_t RightOffSetKey_LastTemp = 0xFF ; 
	
	if(!(GPIOD -> IDR & (1<<1))) LefOffSetKey_Temp &= 0xFE ; 
	if(!(GPIOD -> IDR & (1<<0))) LefOffSetKey_Temp &= 0xFD ; 
	if(!(GPIOC -> IDR & (1<<7))) LefOffSetKey_Temp &= 0xFB ; 
	if(!(GPIOC -> IDR & (1<<6))) LefOffSetKey_Temp &= 0xF7 ; 
	if(!(GPIOE -> IDR & (1<<7))) LefOffSetKey_Temp &= 0xEF ; 
	
	if(!(GPIOE -> IDR & (1<<3))) RightOffSetKey_Temp &= 0xFE ; 
	if(!(GPIOE -> IDR & (1<<2))) RightOffSetKey_Temp &= 0xFD ; 
	if(!(GPIOE -> IDR & (1<<0))) RightOffSetKey_Temp &= 0xFB ; 
	if(!(GPIOE -> IDR & (1<<1))) RightOffSetKey_Temp &= 0xF7 ; 
	if(!(GPIOE -> IDR & (1<<6))) RightOffSetKey_Temp &= 0xEF ; 
	
        #warning  确认微调按键是否正确
	if(LefOffSetKey_LastTemp == LefOffSetKey_Temp)
	{
	  	if(OffSetKeyTemp[0][0] < 50) 
		{
			++OffSetKeyTemp[0][0];
			OffSetKeyTemp[0][1] = 0 ; 
			if(OffSetKeyTemp[0][0] == 5) 
			{
			  	if(LefOffSetKey_Temp == 0xFE) 		OffSetKeyValue = __stKey_R_Plus ;//左边五维按键 : 上
				else if(LefOffSetKey_Temp == 0xFD) 	OffSetKeyValue = __stKey_R_Sub ; //左边五维按键 : 上
				else if(LefOffSetKey_Temp == 0xFB) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_T_Plus ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_E_Plus ;  //左边五维按键 : 上
				}
				else if(LefOffSetKey_Temp == 0xF7) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_T_Sub ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_E_Sub ;  //左边五维按键 : 上
				}
			}
		}
		else
		{
		  	//触发按键连发
		 	if(OffSetKeyTemp[0][1] < 15) ++OffSetKeyTemp[0][1];	
			else  
			{
			  	OffSetKeyTemp[0][1] = 0 ; 
			  	if(LefOffSetKey_Temp == 0xFE) 		OffSetKeyValue = __stKey_R_Plus ;//左边五维按键 : 上
				else if(LefOffSetKey_Temp == 0xFD) 	OffSetKeyValue = __stKey_R_Sub ; //左边五维按键 : 上
				else if(LefOffSetKey_Temp == 0xFB) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_T_Plus ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_E_Plus ;  //左边五维按键 : 上
				}
				else if(LefOffSetKey_Temp == 0xF7) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_T_Sub ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_E_Sub ;  //左边五维按键 : 上
				}
			} 
		}
	}
	else
	{
	  	LefOffSetKey_LastTemp = LefOffSetKey_Temp ; 
		OffSetKeyTemp[0][0] = 0 ; 
		OffSetKeyTemp[0][1] = 0 ;
	}
	
	
	if(RightOffSetKey_LastTemp == RightOffSetKey_Temp)
	{
	  	if(OffSetKeyTemp[1][0] < 50) 
		{
			++OffSetKeyTemp[1][0];
			OffSetKeyTemp[1][1] = 0 ; 
			if(OffSetKeyTemp[1][0] == 5) 
			{
			  	if(RightOffSetKey_Temp == 0xFE) 	OffSetKeyValue = __stKey_A_Plus ;//左边五维按键 : 上
				else if(RightOffSetKey_Temp == 0xFD) 	OffSetKeyValue = __stKey_A_Sub ; //左边五维按键 : 上
				else if(RightOffSetKey_Temp == 0xFB) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_E_Plus ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_T_Plus ;  //左边五维按键 : 上
				}
				else if(RightOffSetKey_Temp == 0xF7) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_E_Sub ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_T_Sub ;  //左边五维按键 : 上
				}
			}
		}
		else
		{
		  	//触发按键连发
		 	if(OffSetKeyTemp[1][1] < 15) ++OffSetKeyTemp[1][1];	
			else  
			{
			  	OffSetKeyTemp[1][1] = 0 ; 
			  	if(RightOffSetKey_Temp == 0xFE) 	OffSetKeyValue = __stKey_A_Plus ;//左边五维按键 : 上
				else if(RightOffSetKey_Temp == 0xFD) 	OffSetKeyValue = __stKey_A_Sub ; //左边五维按键 : 上
				else if(RightOffSetKey_Temp == 0xFB) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_E_Plus ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_T_Plus ;  //左边五维按键 : 上
				}
				else if(RightOffSetKey_Temp == 0xF7) 	
				{
					if(RFHabit == __AmericaPlayer)	OffSetKeyValue = __stKey_E_Sub ; //左边五维按键 : 上
					else				OffSetKeyValue = __stKey_T_Sub ;  //左边五维按键 : 上
				}
			} 
		}
	}
	else
	{
	  	RightOffSetKey_LastTemp = RightOffSetKey_Temp ; 
		OffSetKeyTemp[1][0] = 0 ; 
		OffSetKeyTemp[1][1] = 0 ;
	}*/
}

void InversionKey_Scan(void)
{
  	uint8_t InversionKey_Temp = 0 ; 
	static uint8_t  InversionKey_LastTemp = 0 ; 
	static  uint8_t InversionKeySame_cnt  = 0 ; 
	
	//6个反向拨码开关
  	InversionKey_Temp  = (GPIOA -> IDR & 0xF0) ;
	InversionKey_Temp |= (GPIOG -> IDR & 0x0C) ;
	InversionKey_Temp |= (GPIOF -> IDR & 0x03) ;
	
	if(InversionKey_Temp == InversionKey_LastTemp)
	{
		if(InversionKeySame_cnt < 25) ++InversionKeySame_cnt ; 
		else 
		{
		  	//======================================================
		  	//记录当前 反向开关。 启动后，改变反向开关无效，直到下次上电。
		  	//======================================================
		  	ChannelInversion_flg = 0x3F ; 
		  	if(InversionKey_Temp & 0x80) ChannelInversion_flg &= 0xFE ;	//RUDDER
			if(InversionKey_Temp & 0x10) ChannelInversion_flg &= 0xFD ;	//THROTTLE
			if(InversionKey_Temp & 0x40) ChannelInversion_flg &= 0xFB ;	//ELEVATOR
			if(InversionKey_Temp & 0x20) ChannelInversion_flg &= 0xF7 ;	//AILERON
			
			if(InversionKey_Temp & 0x01) ChannelInversion_flg &= 0xEF ;	//AUX1
			if(InversionKey_Temp & 0x02) ChannelInversion_flg &= 0xDF ;	//AUX2
			
		}
	}
	else
	{
		InversionKeySame_cnt = 0 ; 
		InversionKey_LastTemp = InversionKey_Temp ; 
	}
	
	
	//!!!!!!!!!!!!!!!!!!!!! 注意 !!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!如果日本手焊点短接 , 则是日本手。反之是美国手!!!!!!!
	//!!美国手 和 日本手 中位标记   微调按键需要整体映射!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	uint8_t RFHabit_flg = (GPIOC -> IDR & (1<<0)) ;
	static uint8_t RFHabit_Lastflg = 0 ; 
	static uint8_t HabitSameCnt = 0 ; 
	if(RFHabit_flg == RFHabit_Lastflg) 
	{
	  	if(HabitSameCnt < 25) ++HabitSameCnt ; 
		else 
		{
			if(RFHabit_Lastflg)
			{
				RFHabit = __AmericaPlayer ;
			}
			else
			{
				RFHabit = __JapanPlayer ;
			}
		}
	}
	else
	{
	  	RFHabit_Lastflg = RFHabit_flg ;
		HabitSameCnt = 0 ; 
	}
}
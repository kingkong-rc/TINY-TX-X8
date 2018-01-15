#include "include.h"

#define OVERTIME_MAX 15 	//遥控器静止超过 15Min 报警

//摇杆映射曲线
const uint16_t OutputCode[420] = 
{
/*   	0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 
	0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 
	0   , 0   , 0   , 0   , 0   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 1   , 
	1   , 1   , 1   , 1   , 1   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 2   , 3   , 3   , 3   , 3   , 
	3   , 3   , 3   , 3   , 3   , 3   , 4   , 4   , 4   , 4   , 4   , 4   , 4   , 5   , 5   , 5   , 5   , 5   , 5   , 6   , 
	6   , 6   , 6   , 6   , 6   , 7   , 7   , 7   , 7   , 7   , 8   , 8   , 8   , 8   , 8   , 9   , 9   , 9   , 9   , 10  , 
	10  , 10  , 10  , 11  , 11  , 11  , 11  , 12  , 12  , 12  , 13  , 13  , 13  , 13  , 14  , 14  , 14  , 15  , 15  , 15  , 
	16  , 16  , 16  , 17  , 17  , 17  , 18  , 18  , 18  , 19  , 19  , 20  , 20  , 20  , 21  , 21  , 22  , 22  , 22  , 23  , 
	23  , 24  , 24  , 25  , 25  , 26  , 26  , 27  , 27  , 27  , 28  , 28  , 29  , 29  , 30  , 31  , 31  , 32  , 32  , 33  , 
	33  , 34  , 34  , 35  , 35  , 36  , 37  , 37  , 38  , 38  , 39  , 40  , 40  , 41  , 42  , 42  , 43  , 44  , 44  , 45  , 
	46  , 46  , 47  , 48  , 48  , 49  , 50  , 51  , 51  , 52  , 53  , 54  , 54  , 55  , 56  , 57  , 57  , 58  , 59  , 60  , 
	61  , 61  , 62  , 63  , 64  , 65  , 66  , 67  , 68  , 68  , 69  , 70  , 71  , 72  , 73  , 74  , 75  , 76  , 77  , 78  , 
	79  , 80  , 81  , 82  , 83  , 84  , 85  , 86  , 87  , 88  , 89  , 90  , 91  , 92  , 93  , 94  , 96  , 97  , 98  , 99  , 
	100 , 101 , 102 , 104 , 105 , 106 , 107 , 108 , 110 , 111 , 112 , 113 , 115 , 116 , 117 , 118 , 120 , 121 , 122 , 124 , 
	125 , 126 , 128 , 129 , 130 , 132 , 133 , 135 , 136 , 137 , 139 , 140 , 142 , 143 , 145 , 146 , 148 , 149 , 151 , 152 , 
	154 , 155 , 157 , 158 , 160 , 162 , 163 , 165 , 166 , 168 , 170 , 171 , 173 , 175 , 176 , 178 , 180 , 181 , 183 , 185 , 
	187 , 188 , 190 , 192 , 194 , 196 , 197 , 199 , 201 , 203 , 205 , 207 , 208 , 210 , 212 , 214 , 216 , 218 , 220 , 222 , 
	224 , 226 , 228 , 230 , 232 , 234 , 236 , 238 , 240 , 242 , 244 , 246 , 248 , 251 , 253 , 255 , 257 , 259 , 261 , 264 , 
	266 , 268 , 270 , 272 , 275 , 277 , 279 , 282 , 284 , 286 , 289 , 291 , 293 , 296 , 298 , 300 , 303 , 305 , 308 , 310 , 
	313 , 315 , 318 , 320 , 323 , 325 , 328 , 330 , 333 , 335 , 338 , 340 , 343 , 346 , 348 , 351 , 354 , 356 , 359 , 362 , 
	365 , 367 , 370 , 373 , 376 , 378 , 381 , 384 , 387 , 390 , 393 , 395 , 400 , 401 , 404 , 407 , 410 , 413 , 416 , 419 , */
  
  
  
          //////////////////////////////////////////////////////////////////////////////////////////
          ///////// 下面是11月18日新优化的曲线，缩小了曲率
          //////////////////////////////////////////////////////////////////////////////////////////
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   3,   3,
	  3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,
	  9,  10,  10,  10,  11,  11,  12,  12,  13,  13,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,
	 19,  19,  20,  20,  21,  21,  22,  22,  23,  23,  24,  24,  25,  25,  26,  26,  27,  27,  28,  28,
	 29,  29,  30,  30,  31,  31,  32,  32,  33,  33,  34,  34,  35,  35,  36,  36,  37,  37,  38,  39,
	 39,  40,  41,  41,  42,  43,  43,  44,  45,  45,  46,  47,  47,  48,  49,  49,  50,  51,  52,  53,
	 54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,
	 74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,
	 94,  96,  97,  99, 100, 102, 103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 118, 120, 121, 123,
	124, 126, 127, 128, 130, 131, 133, 134, 135, 137, 138, 140, 141, 143, 144, 146, 147, 149, 150, 152,
	153, 155, 156, 158, 159, 161, 162, 164, 165, 165, 167, 168, 170, 171, 173, 174, 176, 177, 179, 180,
	182, 183, 185, 186, 188, 189, 191, 192, 194, 195, 197, 198, 200, 201, 203, 204, 206, 207, 209, 210,
	212, 213, 215, 216, 218, 219, 221, 222, 224, 225, 227, 228, 230, 231, 233, 234, 236, 237, 239, 241,
	242, 244, 245, 247, 248, 250, 251, 253, 254, 256, 257, 259, 260, 262, 263, 265, 266, 268, 269, 271,
	272, 274, 275, 277, 278, 280, 281, 283, 284, 286, 287, 289, 290, 292, 293, 295, 296, 298, 299, 301,
	302, 304, 305, 307, 308, 310, 311, 313, 314, 316, 318, 319, 321, 322, 324, 325, 327, 328, 330, 331,
	333, 334, 336, 337, 339, 340, 342, 343, 345, 346, 348, 349, 351, 352, 354, 355, 357, 358, 360, 361, 
	363, 364, 366, 367, 369, 370, 372, 373, 375, 376, 378, 379, 381, 382, 384, 385, 387, 388, 390, 392, 
	393, 395, 396, 398, 399, 401, 402, 404, 405, 407, 408, 410, 411, 413, 414, 416, 417, 419, 420, 420, 

};

uint8_t  ChannelInversion_flg = 0x00 ; 	//通道反向标志位(物理采样正常计算，发送时反向)
//摇杆相关变量
//最大 12 - CH                    RUD        /  THR   / ELE       /     ALL    /   Gear    /   Aux1    /   Aux2    /   Aux3    /   Aux4    /   Aux5    /   Aux6    /   Aux7  
uint16_t Sampling_Data[12]  = {  Input_Mid  ,   0 ,  Input_Mid   , Input_Mid  , Input_Mid , Input_Mid , Input_Mid , Input_Mid , Input_Mid , Input_Mid , Input_Mid , Input_Mid};	
//四摇杆相关变量(最大、最小、中位值)
uint16_t Sampling_MaxMinData[4][3] = 
{
	{Input_Max , Input_Mid , Input_Min},
	{Input_Max , Input_Mid , Input_Min},
	{Input_Max , Input_Mid , Input_Min},
	{Input_Max , Input_Mid , Input_Min},
};
uint8_t  Sampling_Offset[4] = {50   , 50   , 50   , 50};			//四摇杆偏置值(0-100)
 
//==============================================================================
//通过采样值，换算发送值
//==============================================================================
uint16_t Get_SendValue(ChannelTypeDef Channel)
{
  	uint16_t ADTemp = 0 ; 
	uint16_t OutputTemp = 0 ; 
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!只有三个摇杆数据需要和中位值计算(除油门数据外)!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!千万注意，中位数组别越界!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if((Channel == RUDDER) || (Channel == ELEVATOR) || (Channel == AILERON))
	{	
	  	//判断标定极值值范围，防止除数等于零的情况
	  	if(Sampling_MaxMinData[Channel][MAXDAT] < MAXValue_Min)         Sampling_MaxMinData[Channel][MAXDAT] = MAXValue_Min ; 
		if(Sampling_MaxMinData[Channel][MINDAT] > MINValue_Max)         Sampling_MaxMinData[Channel][MINDAT] = MINValue_Max ; 
	  	if(Sampling_MaxMinData[Channel][MIDDAT] > AD_MidValue_Max)      Sampling_MaxMinData[Channel][MIDDAT] = AD_MidValue_Max ; 
		else if(Sampling_MaxMinData[Channel][MIDDAT] < AD_MidValue_Min) Sampling_MaxMinData[Channel][MIDDAT] = AD_MidValue_Min ; 
		
		//限定采样AD有效范围(标定最小值 - 标定最大值)
		if(Sampling_Data[Channel] > Sampling_MaxMinData[Channel][MAXDAT])     ADTemp = Sampling_MaxMinData[Channel][MAXDAT] ; 
		else if(Sampling_Data[Channel] < Sampling_MaxMinData[Channel][MINDAT])ADTemp = Sampling_MaxMinData[Channel][MINDAT] ; 
		else								      ADTemp = Sampling_Data[Channel] ; 
		
		//将三个摇杆输出数据做限幅处理(上下幅值各砍 100)
	  	if(ADTemp >= Sampling_MaxMinData[Channel][MIDDAT]) { OutputTemp = Output_Mid + (uint16_t)((ADTemp - Sampling_MaxMinData[Channel][MIDDAT]) * (((float)(Output_Max - Output_Mid - 100))/(Sampling_MaxMinData[Channel][MAXDAT] - Sampling_MaxMinData[Channel][MIDDAT])) + 0.5f) ; }
		else { OutputTemp = Output_Mid - (uint16_t)((Sampling_MaxMinData[Channel][MIDDAT] - ADTemp) * (((float)(Output_Mid - Output_Min - 100))/(Sampling_MaxMinData[Channel][MIDDAT] - Sampling_MaxMinData[Channel][MINDAT])) + 0.5f)  ; }
		
		//查表换算前，检查数据范围防止越界(查表范围不能超过412)
		if(OutputTemp > (Output_Max - 100)) OutputTemp = (Output_Max - 100) ; 
		if(OutputTemp < (Output_Min + 100)) OutputTemp = (Output_Min + 100) ;
		if(OutputTemp > Output_Mid) OutputTemp = OutputCode[OutputTemp - Output_Mid] + Output_Mid;
		else		            OutputTemp = Output_Mid - OutputCode[Output_Mid - OutputTemp];
		
		//计算偏置情况
		if(Sampling_Offset[Channel] > 100)   Sampling_Offset[Channel] = 100 ;
		OutputTemp += Sampling_Offset[Channel] * 2 ; 
		//防止数据减完小于 100 
		if(OutputTemp > 200) OutputTemp -= 100 ;
		else OutputTemp = 100 ; 
		
		//再次检查发射数据范围
		if(OutputTemp > (Output_Max - 100)) OutputTemp = (Output_Max - 100); 
		if(OutputTemp < (Output_Min + 100)) OutputTemp = (Output_Min + 100) ;
		
	}
	
	//其他没有中位值的通道，正常计算 
	else
	{
		
		if(Channel == THROTTLE)
		{
		  	//判断标定极值值范围，防止除数等于零的情况
	  		if(Sampling_MaxMinData[Channel][MAXDAT] < MAXValue_Min)         Sampling_MaxMinData[Channel][MAXDAT] = MAXValue_Min ; 
			if(Sampling_MaxMinData[Channel][MINDAT] > MINValue_Max)         Sampling_MaxMinData[Channel][MINDAT] = MINValue_Max ;
			//限定采样AD有效范围(标定最小值 - 标定最大值)
			if(Sampling_Data[Channel] > Sampling_MaxMinData[Channel][MAXDAT])       ADTemp = Sampling_MaxMinData[Channel][MAXDAT] ; 
			else if(Sampling_Data[Channel] < Sampling_MaxMinData[Channel][MINDAT])  ADTemp = Sampling_MaxMinData[Channel][MINDAT] ;
			else								        ADTemp = Sampling_Data[Channel] ; 
		  	OutputTemp = (uint16_t)((ADTemp- Sampling_MaxMinData[Channel][MINDAT]) * ((float)(THR_Output_Max - THR_Output_Min)/(Sampling_MaxMinData[Channel][MAXDAT] - Sampling_MaxMinData[Channel][MINDAT]))+ 0.5f) + THR_Output_Min ;
			//加上偏置值
			if(Sampling_Offset[Channel] > 100)   Sampling_Offset[Channel] = 100 ;
			OutputTemp += Sampling_Offset[Channel] * 2 ; 
			if(OutputTemp > 100) OutputTemp -= 100 ;
			else OutputTemp = 0 ; 
			
			//油门限幅
			if(OutputTemp > THR_Output_Max) OutputTemp = THR_Output_Max ; 
			if(OutputTemp < THR_Output_Min) OutputTemp = THR_Output_Min ;
		}
		else
		{
			OutputTemp = (uint16_t)(Sampling_Data[Channel] * ((float)(Output_Max - Output_Min)/(Input_Max - Input_Min))+ 0.5f) + Output_Min ;
		}
	}
	
	//是否需要反向
	if((Channel < 6) && (ChannelInversion_flg & (1<<Channel)))
	{
		 OutputTemp = 2*Output_Mid - OutputTemp;
	}
	//======================================================================
	//	         防止数据超过最大值
	//======================================================================
	if(OutputTemp > Output_Max) OutputTemp = Output_Max ; 
	if(OutputTemp < Output_Min) OutputTemp = Output_Min ; 
	
	return OutputTemp ; 
}


//ONOFF_flg : 打开关闭通道显示
//true : 当前显示遥控器通道     false : 清零LED显示，方便用于报警显示
void Init_ChannelDis(bool ONOFF_flg)
{
  	LED_State_ON 	&= LED_NONE ; 
	LED_State_Shake &= LED_NONE ; 
  	//显示当前通道档位
	if(ONOFF_flg == true)
	{
		if(Sampling_Data[AUX1]      >= ADC_AUX_HIGH) 	{LED_State_ON &= ~(LED_CH5_M|LED_CH5_L) ; LED_State_ON |= LED_CH5_H ;}
		else 						{LED_State_ON &= ~(LED_CH5_H|LED_CH5_M) ; LED_State_ON |= LED_CH5_L ;}
			
		if(Sampling_Data[AUX2]      >= ADC_AUX_HIGH)    {LED_State_ON &= ~(LED_CH6_M|LED_CH6_L) ; LED_State_ON |= LED_CH6_H ;}
		else if(Sampling_Data[AUX2] >= ADC_AUX_MID) 	{LED_State_ON &= ~(LED_CH6_H|LED_CH6_L) ; LED_State_ON |= LED_CH6_M ;}
		else 						{LED_State_ON &= ~(LED_CH6_H|LED_CH6_M) ; LED_State_ON |= LED_CH6_L ;}
	}
}

//==============================================================================
//设置微调参数
//==============================================================================
void SetOffSet(void)
{
	////////////////////////////////////////////////////////////////////////
	////////			偏置微调			////////
	////////////////////////////////////////////////////////////////////////
  	if(OffSetKeyValue == __stKey_R_Plus)					//RUDDER 偏置 加
	{
		if(Sampling_Offset[RUDDER] < 100) ++Sampling_Offset[RUDDER] ; 
		else Sampling_Offset[RUDDER] = 100 ; 
		
		if(Sampling_Offset[RUDDER] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[RUDDER] == 0)||(Sampling_Offset[RUDDER] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[RUDDER] > 50) beepCmd(1500 - ((Sampling_Offset[RUDDER] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[RUDDER])*10), __stKeyClick);  
			}
		} 
		Write_EE_Byte(Sampling_Offset[RUDDER]   , RUD_OFFSET_ADDR);
	}
	else if(OffSetKeyValue == __stKey_R_Sub)				//RUDDER 偏置 减
	{
		if(Sampling_Offset[RUDDER]) --Sampling_Offset[RUDDER] ; 
		
		if(Sampling_Offset[RUDDER] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[RUDDER] == 0)||(Sampling_Offset[RUDDER] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[RUDDER] > 50) beepCmd(1500 - ((Sampling_Offset[RUDDER] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[RUDDER])*10), __stKeyClick);  
			}
		}
		Write_EE_Byte(Sampling_Offset[RUDDER]   , RUD_OFFSET_ADDR);
	}
  
  	else if(OffSetKeyValue == __stKey_T_Plus)				//THROTTLE 偏置 加
	{
		if(Sampling_Offset[THROTTLE] < 100) ++Sampling_Offset[THROTTLE] ; 
		else Sampling_Offset[THROTTLE] = 100 ; 
		
		if(Sampling_Offset[THROTTLE] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[THROTTLE] == 0)||(Sampling_Offset[THROTTLE] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[THROTTLE] > 50) beepCmd(1500 - ((Sampling_Offset[THROTTLE] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[THROTTLE])*10), __stKeyClick);  
			}
		}
		Write_EE_Byte(Sampling_Offset[THROTTLE]   , THR_OFFSET_ADDR);
	}
	else if(OffSetKeyValue == __stKey_T_Sub)				//THROTTLE 偏置 减
	{
		if(Sampling_Offset[THROTTLE]) --Sampling_Offset[THROTTLE] ; 
		
		if(Sampling_Offset[THROTTLE] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[THROTTLE] == 0)||(Sampling_Offset[THROTTLE] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[THROTTLE] > 50) beepCmd(1500 - ((Sampling_Offset[THROTTLE] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[THROTTLE])*10), __stKeyClick);  
			}
		}
		Write_EE_Byte(Sampling_Offset[THROTTLE]   , THR_OFFSET_ADDR);
	}
	
  	else if(OffSetKeyValue == __stKey_E_Plus)				//ELEVATOR 偏置 加
	{
		if(Sampling_Offset[ELEVATOR] < 100) ++Sampling_Offset[ELEVATOR] ; 
		else Sampling_Offset[ELEVATOR] = 100 ; 
		
		if(Sampling_Offset[ELEVATOR] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[ELEVATOR] == 0)||(Sampling_Offset[ELEVATOR] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[ELEVATOR] > 50) beepCmd(1500 - ((Sampling_Offset[ELEVATOR] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[ELEVATOR])*10), __stKeyClick);  
			}
		}
		Write_EE_Byte(Sampling_Offset[ELEVATOR]   , ELE_OFFSET_ADDR);
	}
	else if(OffSetKeyValue == __stKey_E_Sub)				//ELEVATOR 偏置 减
	{
		if(Sampling_Offset[ELEVATOR]) --Sampling_Offset[ELEVATOR] ; 
		
		if(Sampling_Offset[ELEVATOR] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[ELEVATOR] == 0)||(Sampling_Offset[ELEVATOR] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[ELEVATOR] > 50) beepCmd(1500 - ((Sampling_Offset[ELEVATOR] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[ELEVATOR])*10), __stKeyClick);  
			}
		}
		Write_EE_Byte(Sampling_Offset[ELEVATOR]   , ELE_OFFSET_ADDR);
	}
	
  	else if(OffSetKeyValue == __stKey_A_Plus)				//AILERON 偏置 加
	{
		if(Sampling_Offset[AILERON] < 100) ++Sampling_Offset[AILERON] ; 
		else Sampling_Offset[AILERON] = 100 ; 
		
		if(Sampling_Offset[AILERON] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[AILERON] == 0)||(Sampling_Offset[AILERON] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[AILERON] > 50) beepCmd(1500 - ((Sampling_Offset[AILERON] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[AILERON])*10), __stKeyClick);  
			}
		}
		Write_EE_Byte(Sampling_Offset[AILERON]   , AIL_OFFSET_ADDR);
	}
	else if(OffSetKeyValue == __stKey_A_Sub)				//AILERON 偏置 减
	{
		if(Sampling_Offset[AILERON]) --Sampling_Offset[AILERON] ; 
		
		if(Sampling_Offset[AILERON] == 50) 						{ if(RunStatus == __stNormal) beepCmd(1600 , __stMidOffSet);  }
		else if((Sampling_Offset[AILERON] == 0)||(Sampling_Offset[AILERON] == 100))	{ if(RunStatus == __stNormal) beepCmd(1000 , __stMidADAdjustDown); }
		else 				  						
		{ 
		  	if(RunStatus == __stNormal) 
			{
				if(Sampling_Offset[AILERON] > 50) beepCmd(1500 - ((Sampling_Offset[AILERON] - 50)*10), __stKeyClick);  
				else beepCmd(1500 - ((50 - Sampling_Offset[AILERON])*10), __stKeyClick);  
			}
		}
		Write_EE_Byte(Sampling_Offset[AILERON]   , AIL_OFFSET_ADDR);
	}
}

//静止超时判断
void StaticOverTime(void)
{
	//保存上次摇杆AD采样值，和当前AD比对，如果变化大于40/60，更新(清零静止计时)。否则不变
  	static uint16_t Sampling_HSK_AD_LastValue[5] ; 
	static uint8_t  AUX3AUX4_LastState = 0 ; 
	//静止超时计数
	static uint16_t StaticOverTime_Secondcnt = 0 ; 
	static uint8_t  StaticOverTime_Mincnt	 = 0 ;
	
	uint16_t Temp = 0 ; 
	//比较摇杆5个AD值
	for(uint8_t i = 0 ; i<5 ; i++)
	{	
	  	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  	//!!!!!!!!!!!!! 只需比较四个摇杆和一个电位器AD值!!!!!!!!!!!!!!!!
	  	//!!!!!!!!!!!!!!!!!!!!!!!注意数组越界!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  	if(i < 4)
		{
			if(Sampling_HSK_AD_LastValue[i] > Sampling_Data[i]) Temp = Sampling_HSK_AD_LastValue[i] - Sampling_Data[i] ; 
			else Temp = Sampling_Data[i] - Sampling_HSK_AD_LastValue[i] ; 
		}
		else
		{
			if(Sampling_HSK_AD_LastValue[4] > Sampling_Data[AUX3]) Temp = Sampling_HSK_AD_LastValue[4] - Sampling_Data[AUX3] ; 
			else Temp = Sampling_Data[AUX3] - Sampling_HSK_AD_LastValue[4] ; 	
		}
		
		//==============================================================
		//(1)在没有报警前，AD变化小于40 认为是静止。
		//(2)静止超时报警后，AD变化小于 60 才认为是静止(考虑到蜂鸣器报警时有震动，导致AD变化加大)
		//==============================================================
		if(((RunStatus != __stStaTimeOver) && (Temp > 40))||((RunStatus == __stStaTimeOver)&&(Temp > 60)))
		{
			StaticOverTime_Secondcnt = 0 ; StaticOverTime_Mincnt = 0 ;
			if(i < 4) Sampling_HSK_AD_LastValue[i] = Sampling_Data[i] ; 	 //更新AD值
			else 	  Sampling_HSK_AD_LastValue[4] = Sampling_Data[AUX3] ;   //更新AD值
		}
	}
	
	//判断是否有按键 按下
	if((ChannelKeyValue != __stEmpty_Key) || (OffSetKeyValue != __stOffset_EmptyKey))
	{
		StaticOverTime_Secondcnt = 0 ; StaticOverTime_Mincnt = 0 ;
	}
	
	//判断是否改变了 AUX3 AUX4的拨码位置
	if(AUX3AUX4_LastState != (GPIOG -> IDR & ((1<<2)|(1<<3))))
	{
	  	StaticOverTime_Secondcnt = 0 ; StaticOverTime_Mincnt = 0 ;
		AUX3AUX4_LastState = (GPIOG -> IDR & ((1<<2)|(1<<3))) ; 
	}
	
	//遥控器静止超过 15Min 遥控器报警
	if(StaticOverTime_Mincnt >= OVERTIME_MAX)
	{
	  	//置位前需要判断 当前报警状态优先级 是否低于 静止超时报警
		if(RunStatus < __stStaTimeOver) 
		{
			RunStatus = __stStaTimeOver ;
			beepCmd(NormalFreCounts, __stStaticOverTimeWarning);
		} 
	}
	else
	{
	 	if(StaticOverTime_Secondcnt > 30000) {++StaticOverTime_Mincnt ; StaticOverTime_Secondcnt = 0 ;}
		else ++StaticOverTime_Secondcnt ; 
		
		if(RunStatus == __stStaTimeOver) 
		{
		  	RunStatus = __stNormal ; 
			beepCmd(NormalFreCounts, __stExitStaticOverTime);
		}
	}
	
}

//初始化检测启动时的油门位置(油门发送值大于 电机起转点，不发送，并报警)
static void ST_Init(void)
{
	//启动时 油门发送数据 必须小于 电调起转点(需要注意美国手/日本手  正向/反向 的区别) 
  	uint16_t THR_Temp = Get_SendValue(THROTTLE) ; 
	if((!(ChannelInversion_flg & (1<<THROTTLE))&&(Get_SendValue(THROTTLE) < (Output_Min + ESC_StartUp_Value))) 
	 ||((ChannelInversion_flg & (1<<THROTTLE))&&(Get_SendValue(THROTTLE)  > (Output_Max - ESC_StartUp_Value))))
	{
	  	HighThrottle_flg = false ; 					//清零高油门上电标志位(高油门上电标志位置 1 时，无线发送初始数据)
		MenuCtrl.Sub_RunStep = 1 ; 

		//只在正常状态是有蜂鸣器提示(防止覆盖其他报警状态)
		if((RunStatus == __stNormal) || (RunStatus == __stTHR_err))  
		{
			if(Bind_flg == false) beepCmd(PowerOnFreCounts , __stMidOffSet) ;
			else
			{
				SetBind();
				LED_State_Shake |=  LED_BIND ;
				LED_State_ON    &= ~LED_BIND ; 
				beepCmd(BindFreCounts , __stBindWarning) ;
			}
			RunStatus = __stNormal ; 
		}
		
		
	}
	else
	{
		//油门摇杆高 报警
	  	HighThrottle_flg = true ; 
	  	if(RunStatus < __stTHR_err) //更新状态前，需要判断状态等级。低状态等级不能覆盖高状态等级
		{
		  	beepCmd(NormalFreCounts , __stThrottleHighWarning) ;
			RunStatus = __stTHR_err ; 
		}
	}
}

/*******************************************************************************
初始化完成，开始正常工作
(1) 通道调整 : 其余四方通道可以通过按键设置
*******************************************************************************/
static void ST_StartWork(void)
{
	////////////////////////////////////////////////////////////////////////
	////////			通道调整			////////
  	//注意 : AUX1(1200/1800 -> AD值850/3253)   AUX2(1200 / 1500 / 1800 -> AD值 850 / 2052 / 3253)
	////////////////////////////////////////////////////////////////////////
	if(ChannelKeyValue == __stKey_CH5_Up)	//CH5_UP(1200 / 1800)
	{
	 	if(RunStatus == __stNormal) beepCmd(NormalFreCounts , __stKeyClick);
		
		Sampling_Data[AUX1] = ADC_AUX_HIGH ;  LED_State_ON &= ~(LED_CH5_M|LED_CH5_L) ; LED_State_ON |= LED_CH5_H ; 
		//F**K , 因为 RWW 只支持单字节操作(单字节读写，不影响单片机其他操作，所以把数据拆分成两个单字节数据来操作)
		Write_EE_Byte((Sampling_Data[AUX1] >> 8)   , AUX1VALUE_ADDR);
		Write_EE_Byte((Sampling_Data[AUX1] & 0xFF) , AUX1VALUE_ADDR + 1);
	}
	else if(ChannelKeyValue == __stKey_CH5_Down) 	//CH5_DOWN
	{
		if(RunStatus == __stNormal) beepCmd(NormalFreCounts , __stKeyClick);
		
		Sampling_Data[AUX1] = ADC_AUX_LOW ; LED_State_ON &= ~(LED_CH5_H|LED_CH5_M) ; LED_State_ON |= LED_CH5_L ; 
		//F**K , 因为 RWW 只支持单字节操作(单字节读写，不影响单片机其他操作，所以把数据拆分成两个单字节数据来操作)
		Write_EE_Byte((Sampling_Data[AUX1] >> 8)   , AUX1VALUE_ADDR);
		Write_EE_Byte((Sampling_Data[AUX1] & 0xFF) , AUX1VALUE_ADDR + 1);
	}
	
	else if(ChannelKeyValue == __stKey_CH6_Up)	//CH6_UP
	{
		if(RunStatus == __stNormal) beepCmd(NormalFreCounts , __stKeyClick);
		
		if(Sampling_Data[AUX2] < ADC_AUX_MID)      		{ Sampling_Data[AUX2] = ADC_AUX_MID  ; LED_State_ON &= ~(LED_CH6_H|LED_CH6_L) ; LED_State_ON |= LED_CH6_M ; }
		else 				    			{ Sampling_Data[AUX2] = ADC_AUX_HIGH ; LED_State_ON &= ~(LED_CH6_M|LED_CH6_L) ; LED_State_ON |= LED_CH6_H ; }
		//F**K , 因为 RWW 只支持单字节操作(单字节读写，不影响单片机其他操作，所以把数据拆分成两个单字节数据来操作)
		Write_EE_Byte((Sampling_Data[AUX2] >> 8)   , AUX2VALUE_ADDR);
		Write_EE_Byte((Sampling_Data[AUX2] & 0xFF) , AUX2VALUE_ADDR + 1);
	}
	else if(ChannelKeyValue == __stKey_CH6_Down) 	//CH6_DOWN
	{
		if(RunStatus == __stNormal) beepCmd(NormalFreCounts , __stKeyClick);
		
		if(Sampling_Data[AUX2] > ADC_AUX_MID)      		{ Sampling_Data[AUX2] = ADC_AUX_MID ; LED_State_ON &= ~(LED_CH6_H|LED_CH6_L) ; LED_State_ON |= LED_CH6_M ; }
		else				   	 		{ Sampling_Data[AUX2] = ADC_AUX_LOW ; LED_State_ON &= ~(LED_CH6_H|LED_CH6_M) ; LED_State_ON |= LED_CH6_L ; }
		//F**K , 因为 RWW 只支持单字节操作(单字节读写，不影响单片机其他操作，所以把数据拆分成两个单字节数据来操作)
		Write_EE_Byte((Sampling_Data[AUX2] >> 8)   , AUX2VALUE_ADDR);
		Write_EE_Byte((Sampling_Data[AUX2] & 0xFF) , AUX2VALUE_ADDR + 1);
	}	
	
	//======================================================================
	// 新增 AUX3 / AUX4  对应拨码开关 第 7 / 8 位
	//======================================================================
	if((GPIOG -> IDR & (1<<2)) == 0)
	{
		Sampling_Data[AUX3] = ADC_AUX_HIGH ; 
	}
	else
	{
		Sampling_Data[AUX3] = ADC_AUX_LOW ; 
	}
	
	if((GPIOG -> IDR & (1<<3)) == 0)
	{
		Sampling_Data[AUX4] = ADC_AUX_HIGH ; 
	}
	else
	{
		Sampling_Data[AUX4] = ADC_AUX_LOW ; 
	}
	
	//======================================================================
	//遥控器静止超时判断:摇杆静止超过 15Min 
	//(1)摇杆AD值变化小于 20  (2)且无任何按键按下 认为遥控器静止
	//======================================================================
	StaticOverTime();
}

void (*pST[])(void) = 
{
	ST_Init , 
	ST_StartWork , 
};

void StartUp(void)
{
	pST[MenuCtrl.Sub_RunStep]();
	
	//======================================================================
	//可以在启动后任何时候调整微调参数 : 防止某些时候油门拉至最低位时， 发送值仍然
	//高于起转报警值。 导致死在那里
	//======================================================================
	SetOffSet();
}
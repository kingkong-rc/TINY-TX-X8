#ifndef _LED_H_
#define _LED_H_

//开关LED显示时要注意 : 不包括电源指示灯(电源指示灯只由电源测量函数控制)
#define LED_NONE	0x01	 
#define LED_ALL		0xFE   

#define LED_VOLT	0x01
#define LED_BIND	0x02

#define LED_CH5_H	0x04
#define LED_CH5_M	0x08
#define	LED_CH5_L	0x10

#define LED_CH6_H	0x20
#define LED_CH6_M	0x40
#define LED_CH6_L	0x80

extern uint8_t LED_State_ON     ;
extern uint8_t LED_State_Shake  ; 
extern uint8_t LED_Status_SendDat ; 

void LED_UART1_Init(void);
void LED_Usart1Send(void);
void Volt_LED(float Volt_Value);

#endif

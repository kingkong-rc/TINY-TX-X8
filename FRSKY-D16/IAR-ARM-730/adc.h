#ifndef _ADC_H_
#define _ADC_H_


// AUX 高低档位 AD值是通过 输出值和输入值比例计算得到 : 
// High = 上位机显示 1800 -> (1800 - 988)/1024 * 4094 = 3246.4
// Mid  = 上位机显示 1500 -> (1500 - 988)/1024 * 4094 = 2047
// Low  = 上位机显示 1200 -> (1200 - 988)/1024 * 4094 = 847.58
#define ADC_AUX_LOW	848
#define ADC_AUX_MID	2047
#define ADC_AUX_HIGH	3246

extern float VoltValue ; 
extern bool ISCJ431OKFlg ; 
extern uint16_t ADC_Value[30] ; 

void adc_Init(void);
void GetADValue(void);
uint16_t GetRandom(void) ; 
void StartupAgain_ADC(void);

#endif

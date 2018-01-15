#ifndef __TBC_H
#define __TBC_H

#define FeedTheDog()	{IWDG -> KR = 0xAA ;}

extern volatile bool tbc_2ms_flag ;
extern uint8_t TIM2_ErrorCnt ; 

void tbc_init (void);
void WatchDog_Init(void);
void tbc_t2_Updatisr (void);
void tbc_t3_Updatisr(void);

#endif
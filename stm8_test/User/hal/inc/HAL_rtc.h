#ifndef _HAL_RTC_H
#define _HAL_RTC_H
#include "stm8l15x.h"



typedef struct 
{
	//---RTC的计数和标志
  u16 RTC_count;
  u8 RTC_T1SFlag;
  u8 RTC_T3SFlag;
		
}HALRTC_STRUCT;

extern HALRTC_STRUCT HalRtc_StrPar;


void RTC_Periph_Init(void);
void RTC_restart(unsigned short s);
void EnterHalt(unsigned short s);


#endif


#ifndef _HAL_TIM_H_
#define _HAL_TIM_H_


//-----------------------------------------------全局变量
typedef struct 
{
	//---定时器4的计数和标志
  u16 TIM_count;
  bool TIM_T100mSFlag;
  bool TIM_T200mSFlag;
  bool TIM_T500mSFlag;
  u8 TIM_T1SFlag;
  u8 TIM_T3SFlag;
  u8 TIM_T1minFlag;

}Tim_STRUCT;

extern Tim_STRUCT  HalTim4_StrPar,HalTim2_StrPar;



void HAL_TIM4_config(void);
void HAL_TIM4_Value(void);



#endif
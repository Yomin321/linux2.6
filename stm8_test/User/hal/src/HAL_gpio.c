#include "main.h"


void HAL_GPIO_config(void)
{
  GPIO_Init(GPIOE,GPIO_Pin_1,GPIO_Mode_In_FL_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_1,EXTI_Trigger_Falling);
  
  GPIO_Init(GPIOC,GPIO_Pin_4,GPIO_Mode_Out_PP_High_Fast);
  
  SPIDER_TM_RESET_DISABLE();
}


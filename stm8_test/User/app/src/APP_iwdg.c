#include "APP_iwdg.h"


static IWDG_Control_Struct g_iwdg_control_struct = {0};


void IWDG_config(void)//reset after 40ms.
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  
  IWDG_SetReload(0xFF);
  
  IWDG_Enable(); 
}

void add_IWDG_counter(void)
{
  if(g_iwdg_control_struct.feed_dog_switch == OPEN_FEED_DOG_SWITCH)
  {
    if(g_iwdg_control_struct.IWDG_counter < MAX_FEED_DOG_TIME)
    {
      IWDG_ReloadCounter();
      g_iwdg_control_struct.IWDG_counter++;
    }
  }
}

void set_IWDG_switch(uint8_t value)
{
  g_iwdg_control_struct.feed_dog_switch = value;
}

void feed_IWDG(void)
{
  g_iwdg_control_struct.IWDG_counter = 0;
}



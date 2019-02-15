#ifndef _APP_IWDG_H
#define _APP_IWDG_H


#include "stm8l15x.h"
#include "stm8l15x_iwdg.h"


#define MAX_FEED_DOG_TIME 180000


typedef enum
{
  OPEN_FEED_DOG_SWITCH,
  CLOSE_FEED_DOG_SWITCH
}Feed_Dog_Switch_enum;


typedef struct
{
  uint8_t  feed_dog_switch;
  uint32_t IWDG_counter;
}IWDG_Control_Struct;


void IWDG_config(void);
void add_IWDG_counter(void);
void set_IWDG_switch(uint8_t value);
void feed_IWDG(void);


#endif
#ifndef _MAIN_H
#define _MAIN_H


//-----头文件----
#include "stm8l15x.h"
#include "stm8l15x_it.h"
#include "stm8l15x_pwr.h"
#include <stdio.h>


#include "HAL_usart.h"
#include "HAL_gpio.h"
#include "HAL_tim.h"
#include "HAL_key.h"
#include "HAL_led.h"
#include "HAL_eeprom.h"

#include "APP_nor_mode.h"
#include "APP_spider.h"
#include "APP_iwdg.h"
#include "APP_Camera.h"
#include "APP_flash.h"
#include "APP_led.h"
#include "APP_Screen.h"
#include "APP_Ultrasonic.h"

#include "sysclk.h"

#include "FUN_algorithm.h"
#include "PRO_spider.h"

//--ST库头文件--
#include "stm8l15x_gpio.h"
#include "stm8l15x_usart.h"
#include "stm8l15x_spi.h"



//调试开关
#define	DEBUG_MCU_TO_DDA
#define	DEBUG_MCU_STATUS
#define DEBUG_DDA_TO_MCU
#define DEBUG_MCU_TO_SENSOR

#define _BV(bit) (1 << (bit))


#define USART_MAX_DATA_SIZE		200


#define SPIDER_CMD_TIME  800
#define SPIDER_CMD1_TIME 800

typedef struct 
{
  u8 CmdStartByte[2];
  u8 Init_LED_ShakeFlag;				//ALL LED闪烁标志
  u8 Init_LED_AlternateShakeFlag;		//LED交替闪烁标志
  u8 Init_LED1_ShakeFlag;				//LED1闪烁标志

}Main_STRUCT;


extern Main_STRUCT  Main_StrPar;


#endif

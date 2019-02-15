
#ifndef _APP_LED_H_
#define _APP_LED_H_


//--------引导牌地址
#define Led_showAddr   0X05

//--------STM8L151C8内部EEPROM存储空车位数的地址
#define  VEHICLE_EEPADDR 0x01010

typedef struct
{
	 u16 vehicle_count;	//空余车位计数

}APP_LED_STRUCT;

extern APP_LED_STRUCT APP_LED;


void APP_LED_ValueInit(void);
void APP_LedShow_init(void);
void APP_LED_ShowDataReady(void);
void APP_LEDSetShow(void);

#endif


































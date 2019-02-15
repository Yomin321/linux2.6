

#ifndef _HAL_KEY_H_
#define _HAL_KEY_H_



#define KEY_GPIO		GPIOA
#define KEY1_GPIO_PIN	       GPIO_Pin_7
#define KEY2_GPIO_PIN	       GPIO_Pin_6


#define KEY1_INPUT()	       GPIO_ReadInputDataBit(KEY_GPIO ,KEY1_GPIO_PIN)
#define KEY2_INPUT()	       GPIO_ReadInputDataBit(KEY_GPIO ,KEY2_GPIO_PIN)


#define KEY_PRESS_SCAN_CNT 30		// 按下扫描计数
#define KEY_FLOAT_SCAN_CNT 5		// 浮空扫描计数

typedef struct
{
	u16 press_cnt;
	u16 nopre_cnt;
	u8  press_status;
}KEY_STRUCT;

extern KEY_STRUCT HalKey_StrPar;

void HAL_KEY_config(void);
void HAL_KEY_Value(void);
void HAL_KEY_Scan(void);
void HAL_KEY_deal(void);



#endif







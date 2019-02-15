#include "main.h"


Tim_STRUCT  HalTim4_StrPar;

/*
 * 函 数 名: DEVICE_init
 * 功    能: 定时器4初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void HAL_TIM4_config(void){
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);	//使能TIM4时钟
	TIM4_TimeBaseInit(TIM4_Prescaler_128,125);					//设置定时器频率和计数值
	TIM4_ITConfig(TIM4_IT_Update,ENABLE);						//使能定时器中断
	TIM4_Cmd(ENABLE);											//使能定时器
}

/*
 * 函 数 名: HAL_TIM4_Value
 * 功    能: TIM4结构体赋初始值
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void HAL_TIM4_Value(void){
	HalTim4_StrPar.TIM_count = 0;			//清除定时器计数
    HalTim4_StrPar.TIM_T100mSFlag = FALSE;//清除100ms标志位
	HalTim4_StrPar.TIM_T200mSFlag = FALSE;	//清除200ms标志位
	HalTim4_StrPar.TIM_T500mSFlag = FALSE;	//清除500ms标志位
	HalTim4_StrPar.TIM_T1SFlag = FALSE;	//清除1s标志位
	HalTim4_StrPar.TIM_T3SFlag = FALSE;	//清除3s标志位
	HalTim4_StrPar.TIM_T1minFlag = FALSE;//清除1min标志位
}


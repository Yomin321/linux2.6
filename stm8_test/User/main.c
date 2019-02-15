#include "main.h"

Main_STRUCT  Main_StrPar;

/*
 * 函 数 名: DEVICE_init
 * 功    能: 外围设备初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */    
static void DEVICE_init(void){
	SYSCLK_HSI_Init();		//系统时钟初始化 16MHz HSI
	HAL_TIM4_config();		//定时器4配置 1ms中断
	HAL_LED_config();		//LED初始化 PD7 PD6    低电平有效
	HAL_USART_config();		//串口初始化 USART1:debug PC2 PC3	USART2:485 PE3 PE4	  USART3:STM PE6 PE7
	HAL_KEY_config();		//KEY初始化 PA7	低电平有效
	HAL_GPIO_config();		//STM复位 PC4	STM中断 PE1
	HAL_eeprom_Init();
	//IWDG_config();			//使能独立看门狗
	enableInterrupts();		//使能所有中断
	HAL_LED_AllOff();		//关闭所有LED
}

/*
 * 函 数 名: WORK_VALUE_init
 * 功    能: 结构体赋值初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
static void WORK_VALUE_init(void){
	Main_StrPar.Init_LED_ShakeFlag = TRUE;
	Main_StrPar.Init_LED_AlternateShakeFlag = FALSE;
	Main_StrPar.Init_LED1_ShakeFlag=FALSE;
	
	HAL_KEY_Value();		//按键结构体赋值
	HAL_TIM4_Value();		//定时器4结构体赋值 
	APP_spider_ValueInit(); //spider结构体赋值
	APP_Ultrasonic_ValueInit();
	// APP_Camera_ValueInit();

	APP_LED_ValueInit();	//引导屏空车位数初始化
}

/*
 * 函 数 名: main
 * 功    能: 车位视频主函数
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */

void main(void){
	DEVICE_init();					//外设初始化 
	WORK_VALUE_init();				//结构体赋值初始化
	APP_nor_mode_SysNormalMode();	//系统正常模式
}


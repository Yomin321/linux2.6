#include "main.h"

/*
 * 函 数 名: SYSCLK_HSI_Init
 * 功    能: 初始化系统时钟为HSI
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void SYSCLK_HSI_Init(void)
{
  CLK_HSICmd(ENABLE);
  CLK_LSICmd(ENABLE);
  
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  
  CLK_MainRegulatorCmd(ENABLE);
  
  CLK_HaltConfig(CLK_Halt_FastWakeup, DISABLE);
  CLK_HaltConfig(CLK_Halt_SlowWakeup, ENABLE);
  
  while(PWR_GetFlagStatus(PWR_FLAG_VREFINTF) != SET);
  
  PWR_FastWakeUpCmd(ENABLE);
  PWR_UltraLowPowerCmd(ENABLE);
}

/*
 * 函 数 名: SYSCLK_HSE_Init
 * 功    能: 初始化系统时钟为HSE
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void SYSCLK_HSE_Init(void){
	CLK->ECKCR |= (1<<0);				//打开HSE时钟
	while(CLK->ECKCR & (1<<1) == 0);	//循环LSI启动未准备好状态
	while(CLK->SWCR & (1<<0) == 1);	//循环时钟未切换好状态
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);	//设置分频系数
	CLK->SWR = 0X04;					//设置目标系统时钟为HSE
	CLK->SWCR |= (1<<1);				//开始切换时钟
	while(CLK->SWCR & (1<<0) == 1);	//循环时钟未切换好状态
	CLK->ICKCR &= ~(1<<0);				//关闭HSI时钟
}


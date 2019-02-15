#include "main.h"

KEY_STRUCT HalKey1_StrPar;
KEY_STRUCT HalKey2_StrPar;

/*
 * 函 数 名: HAL_KEY_config
 * 功    能: 按键引脚的初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void HAL_KEY_config(void){
  GPIO_Init(KEY_GPIO, KEY1_GPIO_PIN, GPIO_Mode_In_PU_No_IT);//上拉输入、无中断
}

/*
 * 函 数 名: HAL_KEY_Value
 * 功    能: 按键结构体赋初始值
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void HAL_KEY_Value(void){
	HalKey1_StrPar.press_cnt =0;		//清除按下次数
	HalKey1_StrPar.nopre_cnt =0;		//清除松开次数
	HalKey1_StrPar.press_status =FALSE;	//清除按下状态

	
	// HalKey2_StrPar.press_cnt =0;		//清除按下次数
	// HalKey2_StrPar.nopre_cnt =0;		//清除松开次数
	// HalKey2_StrPar.press_status =FALSE;	//清除按下状态
}

/*
 * 函 数 名: HAL_KEY_Scan
 * 功    能: 按键扫描
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void HAL_KEY_Scan(void){
	if( KEY1_INPUT()==0 ){
		HalKey1_StrPar.press_cnt++;
		HalKey1_StrPar.nopre_cnt=0;
	}
	else{
		HalKey1_StrPar.nopre_cnt++;
	}

	// if( KEY2_INPUT()==0 ){
	// 	HalKey2_StrPar.press_cnt++;
	// 	HalKey2_StrPar.nopre_cnt=0;
	// }
	// else{
	// 	HalKey2_StrPar.nopre_cnt++;
	// }
}

/*
 * 函 数 名: HAL_KEY_deal
 * 功    能: 按键处理
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void HAL_KEY_deal(void){
	//u8 t_buffer[17] = {0x0A,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
	u8 s_tmp[50] = {0};
	//短按处理 LED1闪一下 (检测到200ms内按键都被按下)
	if((HalKey1_StrPar.press_cnt>=KEY_PRESS_SCAN_CNT+100)&&(HalKey1_StrPar.press_cnt<3000)&&(HalKey1_StrPar.nopre_cnt<KEY_FLOAT_SCAN_CNT)){
		if(HalKey1_StrPar.press_status != 1){
			HalKey1_StrPar.press_status=1;
			HalKey1_StrPar.nopre_cnt=0;
			LED1_ON();
		}
		if(HalKey1_StrPar.press_cnt>=1500){
			LED1_OFF();
		}
	}
	//长按处理 LED1闪两下(被按下2s以上)
	else if((HalKey1_StrPar.press_cnt>=KEY_PRESS_SCAN_CNT+2000)&&(HalKey1_StrPar.press_cnt<5000)&&(HalKey1_StrPar.nopre_cnt<KEY_FLOAT_SCAN_CNT))
	{
		if(HalKey1_StrPar.press_status!=2)
		{
			HalKey1_StrPar.press_status=2;
			HalKey1_StrPar.nopre_cnt=0;
			LED1_ON();
		}
		if(HalKey1_StrPar.press_cnt>=5000)
		{
			LED1_OFF();
		}
	}//松开检测
	else if((HalKey1_StrPar.nopre_cnt>=KEY_FLOAT_SCAN_CNT)&&((HalKey1_StrPar.press_status==1) || (HalKey1_StrPar.press_status==2) ))
	{
		HalKey1_StrPar.press_cnt=0;
		HalKey1_StrPar.nopre_cnt=0;
        
		LED1_OFF();		
		//--------处理	单终端组网命令 {0x59,0x03,0x00,0x03,0x11,0x01,0x11,0x47};多终端组网命令  {0x59,0x03,0x00,0x03,0x11,0x02,0x14,0x47};
		if(HalKey1_StrPar.press_status ==1)//短按KEY1 多终端组网
		{
// 			if(APP_LED.vehicle_count == 0)
// 			{
// 				APP_LED.vehicle_count = 1;
// 			}
// 			APP_LED.vehicle_count--;
// #ifdef DEBUG_MCU_STATUS
// 			printf("\r\n空车位数：%d\n",APP_LED.vehicle_count);
// #endif
			
			s_tmp[0] = 0x02;
			PRO_spider_BuildCMDForPar(CMD_TYPE_CTRL,CTRL_CMD_SWITCHWORKMODE,s_tmp,1);
			HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
			HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,USER_USART1);
			APP_SPIDER.DealSendCmdToSpiderAddr = 1;
		}
		else if(HalKey1_StrPar.press_status ==2)//长按KEY1 一级单终端组网
		{
			s_tmp[0] = 0x01;
            PRO_spider_BuildCMDForPar(CMD_TYPE_CTRL,CTRL_CMD_SWITCHWORKMODE,s_tmp,1);
			HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
			HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,USER_USART1);
            APP_SPIDER.DealSendCmdToSpiderAddr = 1;
		}
		HalKey1_StrPar.press_status=0;
	}

// 	//KEY2处理
// 	if((HalKey2_StrPar.press_cnt>=KEY_PRESS_SCAN_CNT+100)&&(HalKey2_StrPar.press_cnt<2000)&&(HalKey2_StrPar.nopre_cnt<KEY_FLOAT_SCAN_CNT)){
// 		if(HalKey2_StrPar.press_status != 1){
// 			HalKey2_StrPar.press_status=1;
// 			HalKey2_StrPar.nopre_cnt=0;
// 			LED2_ON();
// 		}
// 		if(HalKey2_StrPar.press_cnt>=1500){
// 			LED2_OFF();
// 		}
// 	}//短按处理
// 	else if((HalKey2_StrPar.press_cnt>=KEY_PRESS_SCAN_CNT+2000)&&(HalKey2_StrPar.press_cnt<5000)&&(HalKey2_StrPar.nopre_cnt<KEY_FLOAT_SCAN_CNT))
// 	{
// 		if(HalKey2_StrPar.press_status!=2)
// 		{
// 			HalKey2_StrPar.press_status=2;
// 			HalKey2_StrPar.nopre_cnt=0;
// 			LED2_ON();
// 		}
// 		if(HalKey2_StrPar.press_cnt>=5000)
// 		{
// 			LED2_OFF();
// 		}
// 	}//松开检测
// 	else if((HalKey2_StrPar.nopre_cnt>=KEY_FLOAT_SCAN_CNT)&&((HalKey2_StrPar.press_status==1) || (HalKey2_StrPar.press_status==2) ))
// 	{
// 		HalKey2_StrPar.press_cnt=0;
// 		HalKey2_StrPar.nopre_cnt=0;
    	
// 		LED2_OFF();		
// 		//--------处理	单终端组网命令 {0x59,0x03,0x00,0x03,0x11,0x01,0x11,0x47};多终端组网命令  {0x59,0x03,0x00,0x03,0x11,0x02,0x14,0x47};
// 		if(HalKey2_StrPar.press_status ==1)//短按KEY2 空车位数加一
// 		{
// 			if(APP_LED.vehicle_count == 9)
// 			{
// 				APP_LED.vehicle_count = 8;
// 			}
// 			APP_LED.vehicle_count++;
// #ifdef DEBUG_MCU_STATUS
// 			printf("\r\n空车位数：%d\n",APP_LED.vehicle_count);   
// #endif
// 		}
// 		else if(HalKey2_StrPar.press_status ==2)//长按KEY2 一级多终端组网
// 		{
// 			s_tmp[0] = 0x02;
//                 PRO_spider_BuildCMDForPar(CMD_TYPE_CTRL,CTRL_CMD_SWITCHWORKMODE,s_tmp,1);
// 				HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
// 				HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,USER_USART1);
//                 APP_SPIDER.DealSendCmdToSpiderAddr = 1;
// 		}
// 		HalKey2_StrPar.press_status=0;
// 	}
}



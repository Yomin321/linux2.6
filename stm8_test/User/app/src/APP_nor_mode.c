#include "main.h"

/*
 * 函 数 名: APP_nor_mode_RealTask
 * 功    能: 真正的任务
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
static void APP_nor_mode_RealTask(void){

	// u8 s_tmp[18];
	

	feed_IWDG();				//喂狗
	HAL_KEY_Scan();			//按键扫描
	HAL_KEY_deal();				//按键处理
	APP_spider_uartDeal();		//spider串口接收处理
	APP_Spider_dealSendCmdToSpider();	//发送命令到spider处理
	APP_spider_IEPIN_deal();	//处理spider的中断事件

	APP_Ultrasonic_UartRxDealforWork();	//超声波工作模式下串口接收处理

	// s_tmp[0]=0x0A;
	// s_tmp[1]=0x12;
	
	// s_tmp[2]=0x23;
	
	// s_tmp[3]=0x34;
	
	// s_tmp[4]=0x45;
	// PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDUPACK,s_tmp,13);//发送U包
	// APP_SPIDER.DealSendCmdToSpiderAddr = 1;
					
	//APP_Camera_uartDeal();		//spider串口处理
	//APP_Camera_dealSendCmdToSpider();	//发送命令到spider处理
}

/*
 * 函 数 名: APP_nor_mode_FixedTask
 * 功    能: 系统定时工作任务
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void APP_nor_mode_FixedTask(void){
	/**************** 定时1ms需要执行的任务 ****************/
	
	if(HalTim4_StrPar.TIM_T500mSFlag == TRUE){
		static char cnt=0;
		static u16 time_cnt=0;
		static char up_cnt=0;
		if(cnt%1==0){
			APP_ULTRASONIC.TxCnt++;//发送命令次数
			APP_ULTRASONIC.GW_Down_Cmd_Flag=(APP_ULTRASONIC.TxCnt%8);
			if(up_cnt<14){
			switch(APP_ULTRASONIC.GW_Down_Cmd_Flag){
				case 0:
			    APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);	
			     break;
				case 1:
				APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);
				 break;
				case 2:
				APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);
				break;
				case 3:
				APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);
				break;
				case 4:
				APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);
				break;
				case 5:
				APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);
				break;
				case 6:
				APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);
				break;
				case 7:
				APP_Ultrasonic_FixedPollState(APP_ULTRASONIC.GW_Down_Cmd_Flag);
				up_cnt++;
				break;
				default:					
				break;
				}
				}
		}
		if(time_cnt%120==0){
			HalTim4_StrPar.TIM_T1minFlag = TRUE;
			time_cnt=0;
			up_cnt=0;
			}
		if(APP_ULTRASONIC.TxCnt>250)
			APP_ULTRASONIC.TxCnt=0;
		time_cnt++;
		cnt++;
		HalTim4_StrPar.TIM_T500mSFlag = FALSE;
	}

	/*************** 定时200ms需要执行的任务 ***************/

	/*************** 定时500ms需要执行的任务 ***************/

	/**************** 定时1s需要执行的任务 *****************/
	
}

/*
 * 函 数 名: APP_nor_mode_SysNormalMode
 * 功    能: 系统正常工作模式
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void APP_nor_mode_SysNormalMode(void){
		// while(1){
	// 	if(APP_SN_Init()){
	// 		//break;
	// 		printf("have SN");
	// 	}
	// 	else{
	// 		printf("havn`t SN");
	// 	}
	// // }
	APP_SN_Init();
	APP_spider_init();			//spider模块初始化
	Main_StrPar.Init_LED_ShakeFlag = FALSE;	//初始化完成
	APP_Spider_ReadyOnNet();	//
#ifdef DEBUG_MCU_STATUS
	printf("\r\nall init success\r\n");
#endif
	while(1){
		APP_nor_mode_RealTask();	//实时的任务
		APP_nor_mode_FixedTask();
	}
}


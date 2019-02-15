#include "main.h"

/*
 * �� �� ��: APP_nor_mode_RealTask
 * ��    ��: ����������
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 * ��    ��: 
 */
static void APP_nor_mode_RealTask(void){

	// u8 s_tmp[18];
	

	feed_IWDG();				//ι��
	HAL_KEY_Scan();			//����ɨ��
	HAL_KEY_deal();				//��������
	APP_spider_uartDeal();		//spider���ڽ��մ���
	APP_Spider_dealSendCmdToSpider();	//�������spider����
	APP_spider_IEPIN_deal();	//����spider���ж��¼�

	APP_Ultrasonic_UartRxDealforWork();	//����������ģʽ�´��ڽ��մ���

	// s_tmp[0]=0x0A;
	// s_tmp[1]=0x12;
	
	// s_tmp[2]=0x23;
	
	// s_tmp[3]=0x34;
	
	// s_tmp[4]=0x45;
	// PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDUPACK,s_tmp,13);//����U��
	// APP_SPIDER.DealSendCmdToSpiderAddr = 1;
					
	//APP_Camera_uartDeal();		//spider���ڴ���
	//APP_Camera_dealSendCmdToSpider();	//�������spider����
}

/*
 * �� �� ��: APP_nor_mode_FixedTask
 * ��    ��: ϵͳ��ʱ��������
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 * ��    ��: 
 */
void APP_nor_mode_FixedTask(void){
	/**************** ��ʱ1ms��Ҫִ�е����� ****************/
	
	if(HalTim4_StrPar.TIM_T500mSFlag == TRUE){
		static char cnt=0;
		static u16 time_cnt=0;
		static char up_cnt=0;
		if(cnt%1==0){
			APP_ULTRASONIC.TxCnt++;//�����������
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

	/*************** ��ʱ200ms��Ҫִ�е����� ***************/

	/*************** ��ʱ500ms��Ҫִ�е����� ***************/

	/**************** ��ʱ1s��Ҫִ�е����� *****************/
	
}

/*
 * �� �� ��: APP_nor_mode_SysNormalMode
 * ��    ��: ϵͳ��������ģʽ
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 * ��    ��: 
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
	APP_spider_init();			//spiderģ���ʼ��
	Main_StrPar.Init_LED_ShakeFlag = FALSE;	//��ʼ�����
	APP_Spider_ReadyOnNet();	//
#ifdef DEBUG_MCU_STATUS
	printf("\r\nall init success\r\n");
#endif
	while(1){
		APP_nor_mode_RealTask();	//ʵʱ������
		APP_nor_mode_FixedTask();
	}
}


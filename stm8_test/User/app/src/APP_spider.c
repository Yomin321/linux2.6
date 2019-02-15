#include "main.h"


SPIDER_STRUCT  APP_SPIDER;

u8 U_Packet_CntCache=0;	//网关下发的U包计数缓存(暂时以全局变量形式处理)


//------固化的工作参数------
  u8 SPIDER_WORK_DATA[42] = {0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0xAF,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
			                  0x2C,0x01,0x3C,0x0A, 0x34,0x08,0x07,0x3C, 0x00,0xB4,0x00,0x3C, 0x00,0x2C,0x01,0x3C,
				          0x00,0x5A,0x00,0x00, 0x00,0x0A,0x00,0x1E, 0x00,0x00}; //37to42

/*
 * 函 数 名: APP_spider_ValueInit
 * 功    能: spider结构体赋初始值
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void APP_spider_ValueInit(void){
	APP_SPIDER.InitDealAddr = 0;		//清除spider初始化事件ID
	APP_SPIDER.InitOKFlag = FALSE;		//清除spider初始化完成标志位
	APP_SPIDER.IEPINOK_Flag = FALSE;	//清除spider初始化中断事件标志位
	APP_SPIDER.SingleRegFlag = FALSE;	//清除spider单终端组网标志位
	APP_SPIDER.MultRegFlag = FALSE;	//清除spider多终端组网标志位
	APP_SPIDER.MF_Flag = TRUE;			//清除读取模块形态标志位
	APP_SPIDER.WP_Flag = TRUE;			//清除读取工作参数标志位
	APP_SPIDER.WakeupTimeCnt=0;		//清除唤醒时间计数
	APP_SPIDER.WakeupDealAddr=0;		//清除唤醒事件ID
	APP_SPIDER.FastCmdFlag=FALSE;		//清除快捷令标志位
	APP_SPIDER.FastCmdLEDShakeFlag =FALSE;	//清除快捷令LED闪烁标志位
	APP_SPIDER.Judge_Is_OnNetFlag = FALSE;	//组网完成标志位
	APP_SPIDER.Judge_ReadyOnNetFlag = FALSE;//准备组网标志位
	APP_SPIDER.Judge_SendOnNetCmdFlag = TRUE;//发送组网命令标志位
}

/*
 * 函 数 名: APP_spider_ValueInit
 * 功    能: spider结构体赋初始值
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 * 其    他: 
 */
void APP_spider_UsartDealForInit(void)
{
	u8 i = 0;
	u8 s_tmp[100] = {0};
        
        switch(APP_SPIDER.InitDealAddr){
	case 0:break;
	case 1:
		// 读取模块形态
		if(HalTim4_StrPar.TIM_T200mSFlag==FALSE)break;
		HalTim4_StrPar.TIM_T200mSFlag=FALSE;
                
		PRO_spider_BuildCMDForNoPar(CMD_TYPE_QUERY,QUERY_CMD_GETMODULESHAPE);
#ifdef DEBUG_MCU_TO_DDA      	
				printf("\r\n【MCU to spider】:");
				for(i=0; i<APP_SPIDER.TxUsartLeng; i++)
				{
					printf("%02X ",APP_SPIDER.TxUsartData[i]);
				}
				printf("\r\n");
#endif
		HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);// 模块形态查询
                
		APP_SPIDER.InitDealAddr++;
		APP_SPIDER.UartRevOkFlag=FALSE;
		APP_SPIDER.UartRevOverFlag=FALSE;
		APP_SPIDER.UartRevOverCnt=500;//100
		APP_SPIDER.MF_Flag=FALSE;
		break;
	case 2:
		if(APP_SPIDER.UartRevOkFlag==TRUE){
			APP_SPIDER.UartRevOkFlag=FALSE;
                        APP_SPIDER.UartRevOverCnt=0;
			if(APP_SPIDER.MF_Flag==TRUE){
				APP_SPIDER.InitDealAddr++;
			}
			else{
				APP_SPIDER.InitDealAddr=11; //  跳转到模块形态设置
			}
		}
		if(APP_SPIDER.UartRevOverFlag==TRUE){
			APP_SPIDER.UartRevOverFlag=FALSE;
			APP_SPIDER.InitDealAddr--;	
			HalTim4_StrPar.TIM_T200mSFlag=FALSE;
		}
		break;
	case 3:
		// 读取工作参数
		if(HalTim4_StrPar.TIM_T200mSFlag==FALSE)break;
		HalTim4_StrPar.TIM_T200mSFlag=FALSE;
		s_tmp[0] = 42;
		s_tmp[1] = 0x00;
                
		PRO_spider_BuildCMDForPar(CMD_TYPE_QUERY,QUERY_CMD_GETWORKPARAMETER,s_tmp,2);
#ifdef DEBUG_MCU_TO_DDA      	
				printf("\r\n【MCU to spider】:");
				for(i=0; i<APP_SPIDER.TxUsartLeng; i++)
				{
					printf("%02X ",APP_SPIDER.TxUsartData[i]);
				}
				printf("\r\n");
#endif
		HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);// 工作参数查询
                
		APP_SPIDER.InitDealAddr++;	
		APP_SPIDER.UartRevOkFlag=FALSE;
		APP_SPIDER.UartRevOverFlag=FALSE;
		APP_SPIDER.UartRevOverCnt=500;//100
		break;
	case 4:
		
        if(APP_SPIDER.UartRevOkFlag==TRUE){
			APP_SPIDER.UartRevOkFlag=FALSE;
			APP_SPIDER.InitDealAddr++;
            APP_SPIDER.UartRevOverCnt=0;
		}
		if(APP_SPIDER.UartRevOverFlag==TRUE){
			APP_SPIDER.UartRevOverFlag=FALSE;
			APP_SPIDER.InitDealAddr--;	
			HalTim4_StrPar.TIM_T200mSFlag=FALSE;
		}
		break;
     case 5:
		//比较工作参数是否为设定值
		if(ALG_CmpsConst(&UART3.RxBuff[8],42,SPIDER_WORK_DATA,42)==TRUE)
		{
			APP_SPIDER.InitDealAddr= 17;
		}
		else
		{
			// 比较失败，设置工作参数
			APP_SPIDER.InitDealAddr=13;
                        APP_SPIDER.UartRevOverCnt=0;
		}
        break;
		
  
    case 6:
		if(APP_SPIDER.UartRevOkFlag==TRUE){
			APP_SPIDER.UartRevOkFlag=FALSE;
            APP_SPIDER.UartRevOverCnt=0;
			if(APP_SPIDER.MF_Flag==TRUE){
				APP_SPIDER.InitDealAddr++;
			}
			else{
				APP_SPIDER.InitDealAddr=13; //  跳转到模块形态设置
			}
		}
		if(APP_SPIDER.UartRevOverFlag==TRUE){
			APP_SPIDER.UartRevOverFlag=FALSE;
			APP_SPIDER.InitDealAddr--;	
			HalTim4_StrPar.TIM_T200mSFlag=FALSE;
		}
		break;
	case 7:
		APP_SPIDER.InitOKFlag=TRUE;
		APP_SPIDER.InitDealAddr=0;
		break;
	case 11:
		// 设置为一级形态
		s_tmp[0] = 0x00;
                
		PRO_spider_BuildCMDForPar(CMD_TYPE_CONFIG,CONFIG_CMD_SETMODULECONFIG,s_tmp,1);
		HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);// 工作参数查询
                
		APP_SPIDER.InitDealAddr++;	
		APP_SPIDER.UartRevOkFlag=FALSE;
		APP_SPIDER.UartRevOverFlag=FALSE;
		APP_SPIDER.UartRevOverCnt=500;//100
		break;
	case 12:
		
		if(APP_SPIDER.UartRevOkFlag==TRUE){
			APP_SPIDER.UartRevOkFlag=FALSE;
			APP_SPIDER.InitDealAddr=1;
            APP_SPIDER.UartRevOverCnt=0;
		}
		if(APP_SPIDER.UartRevOverFlag==TRUE){
			APP_SPIDER.UartRevOverFlag=FALSE;
			APP_SPIDER.InitDealAddr--;	
		}
		break;
	case 13:
		// 写入参数
		s_tmp[0] = 42;//37to42
		s_tmp[1] = 0x00;//0x05to0x00
                
		for(i=0;i<42;i++)//37to42
		{
                  s_tmp[i+2] = SPIDER_WORK_DATA[i];
		}
                
		PRO_spider_BuildCMDForPar(CMD_TYPE_CONFIG,CONFIG_CMD_SETWORKPARAMETER,(u8 *)s_tmp,44);//39to44
#ifdef DEBUG_MCU_TO_DDA      	
				printf("\r\n【MCU to spider】:");
				for(i=0; i<APP_SPIDER.TxUsartLeng; i++)
				{
					printf("%02X ",APP_SPIDER.TxUsartData[i]);
				}
				printf("\r\n");
#endif
		HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);// 工作参数查询
                
        APP_SPIDER.InitDealAddr++;	
		APP_SPIDER.UartRevOkFlag=FALSE;
		APP_SPIDER.UartRevOverFlag=FALSE;
		APP_SPIDER.UartRevOverCnt=500;//100
		break;
	case 14:
		if(APP_SPIDER.UartRevOkFlag==TRUE){
			APP_SPIDER.UartRevOkFlag=FALSE;
			APP_SPIDER.InitDealAddr=3;
                        APP_SPIDER.UartRevOverCnt=0;
		}
		if(APP_SPIDER.UartRevOverFlag==TRUE){
			APP_SPIDER.UartRevOverFlag=FALSE;
			APP_SPIDER.InitDealAddr--;	
		}
		break;
	case 15:
		break;
	case 16:
		break;
	case 17:
		// 查询组网档案
		//{0x59,0x04,0x00,0x04,0x06,0x01,0x01,0x07,0x47}
		s_tmp[0] = 0x01;
		s_tmp[1] = 0x01;
		
		PRO_spider_BuildCMDForPar(CMD_TYPE_QUERY,QUERY_CMD_GETNETFILE,s_tmp,2);
              //  printf("MCU TO DDA ");
#ifdef DEBUG_MCU_TO_DDA      	
		printf("\r\n【MCU to spider】:");
		for(i=0; i<APP_SPIDER.TxUsartLeng; i++)
		{
			printf("%02X ",APP_SPIDER.TxUsartData[i]);
		}
		printf("\r\n");
#endif
		HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);// 工作参数查询
                
		APP_SPIDER.InitDealAddr++;	
		APP_SPIDER.UartRevOkFlag=FALSE;
		APP_SPIDER.UartRevOverFlag=FALSE;
		APP_SPIDER.UartRevOverCnt=500;//100
		break;
	case 18:
		if(APP_SPIDER.UartRevOkFlag==TRUE){
			APP_SPIDER.UartRevOkFlag=FALSE;
			APP_SPIDER.InitOKFlag=TRUE;
			APP_SPIDER.InitDealAddr=0;
	        APP_SPIDER.UartRevOverCnt=0;
		}
		if(APP_SPIDER.UartRevOverFlag==TRUE){
			APP_SPIDER.UartRevOverFlag=FALSE;
			APP_SPIDER.InitDealAddr--;	
		}
		break;
	default:break;
  } 
}



void APP_spider_IEPIN_deal(void)
{
 if(APP_SPIDER.IEPINOK_Flag == TRUE)
 {
#ifdef DEBUG_MCU_STATUS
   printf("\r\n【MCU Status】:incoming a event.\r\n");
#endif
   APP_SPIDER.IEPINOK_Flag = FALSE;
   
   PRO_spider_BuildCMDForNoPar(CMD_TYPE_QUERY,QUERY_CMD_GETMODULESTATUS);
//   HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
   APP_SPIDER.DealSendCmdToSpiderAddr = 1;
   
   APP_SPIDER.UartRevOverCnt=500;//100
  }
}



//查询中断事件处理
static void Spider_IEPIN_StatusDeal(unsigned char *data)
{
//-----------工作模式切换事件
  if(data[6] &(1 << 0)){
 	switch(data[7] & 0x0f){
 		case 0x01:		//一级单终端组网模式
#ifdef DEBUG_DDA_TO_MCU                                              
 		printf("\r\n【spider status】:only networking mode\r\n");
#endif
		if(Main_StrPar.Init_LED_ShakeFlag == FALSE){
			Main_StrPar.Init_LED1_ShakeFlag = FALSE;
			Main_StrPar.Init_LED_AlternateShakeFlag = TRUE;	//LED交替闪烁开启
		}
			break;
		case 0x02:		//一级多终端组网模式
#ifdef DEBUG_DDA_TO_MCU                                               
 		printf("\r\n【spider status】:multi-terminal networking mode\r\n");
#endif
		if(Main_StrPar.Init_LED_ShakeFlag == FALSE){
			Main_StrPar.Init_LED1_ShakeFlag = FALSE;
			Main_StrPar.Init_LED_AlternateShakeFlag = TRUE;	//LED交替闪烁开启
		}
			break;
		case 0x03:		//普通模式
#ifdef DEBUG_DDA_TO_MCU                                               
		printf("\r\n【spider status】:normal mode\r\n");
#endif
		if(Main_StrPar.Init_LED_ShakeFlag == FALSE){				//判断初始化是否完成
			Main_StrPar.Init_LED_AlternateShakeFlag = FALSE;	//LED交替闪烁关闭
			Main_StrPar.Init_LED1_ShakeFlag = TRUE;				//LED1秒闪开启
		}
			break;
		case 0x04:		//休眠模式
#ifdef DEBUG_DDA_TO_MCU                                               
		printf("\r\n【spider status】:sleep mode\r\n");
#endif
			break;
		default:
			break;
 	}
  }
   //---------------------有下行类别事件产生  
  if(data[6] &(1 << 5)){
	switch(data[9] >> 4){
		case 0x01:			//下行一级快捷令
		  //查询快捷令
		  PRO_spider_BuildCMDForNoPar(CMD_TYPE_QUERY,QUERY_CMD_GETDOWNSTAIRFALST);
//		  HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
                  APP_SPIDER.DealSendCmdToSpiderAddr = 1;
                  
      	          APP_SPIDER.UartRevOverCnt=500;//100
			break;
		case 0x02: 		//下行一级注销令
			break;
		case 0x03:			//下行一级U包
			 PRO_spider_BuildCMDForNoPar(CMD_TYPE_QUERY,QUERY_CMD_GETDOWNUPACK);
//		 	 HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
                         APP_SPIDER.DealSendCmdToSpiderAddr = 1;
                         
                         APP_SPIDER.UartRevOverCnt=500;//100
			break;
		case 0x04:			//下行一级M包
			 PRO_spider_BuildCMDForNoPar(CMD_TYPE_QUERY,QUERY_CMD_GETDOWNMPACK);
//		 	 HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
   			 APP_SPIDER.DealSendCmdToSpiderAddr = 1;
                         
                         APP_SPIDER.UartRevOverCnt=500;//100
			break;
			
		default:
			break;
	}
  }

  if(data[6] &(1 << 1)) //有单终端组网事件产生
  {
  	switch(data[7] >> 4)
  	{
  		case 0x01:					// 单终端组网开始
  			
			break;
  		case 0x02:					//单终端组网正常结束
			APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                        HAL_LED_AllOff();
			APP_SPIDER.Judge_Is_OnNetFlag = TRUE;	//已经组网
			break;
		case 0x03:					 //单终端组网超时1
			if(APP_SPIDER.Judge_Is_OnNetFlag == TRUE)//已经组网
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
			}
			else
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
				APP_SPIDER.Judge_SendOnNetCmdFlag = TRUE;
			}
			break;
		case 0x04:					 //单终端组网超时2
			if(APP_SPIDER.Judge_Is_OnNetFlag == TRUE)		//已经组网
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
			}
			else
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
				APP_SPIDER.Judge_SendOnNetCmdFlag = TRUE;
			}
			break;
		default:
			break;
  	}
  }
 //--------------------有多终端组网事件产生  
  if(data[6] &(1 << 2)){
    switch(data[8] & 0x0f){
    	case 0x02:				//多终端组网正常结束
    		APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                HAL_LED_AllOff();
			APP_SPIDER.Judge_Is_OnNetFlag = TRUE;	//已经组网
			break;
		case 0x03:				 //多终端组网超时1
			
			if(APP_SPIDER.Judge_Is_OnNetFlag == TRUE)		//已经组网
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
			}
			else
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
				APP_SPIDER.Judge_SendOnNetCmdFlag = TRUE;
			}
			break;
		case 0x04:				//多终端组网超时2
			if(APP_SPIDER.Judge_Is_OnNetFlag == TRUE)		//已经组网
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
			}
			else
			{
				APP_SPIDER.Judge_ReadyOnNetFlag = TRUE;
                                HAL_LED_AllOff();
				APP_SPIDER.Judge_SendOnNetCmdFlag = TRUE;
			}
			break;
		default:
			break;
    }  
  }
//----------------------------上行事件产生
  if(data[6] &(1 << 4)){
	switch(data[9] & 0x0f){
    	case 0x01:			//一级上行U包完成
			break;
    	case 0x02:			//一级上行U包超时
			break;
		case 0x03:		//一级上行M包完成
			break;
		case 0x04:		//一级上行M包超时
			break;
		default:
			break;
	  }
   }
}


/*
  spider初始化程序
*/
void APP_spider_init(void){
	uint16_t i = 0;
	APP_SPIDER.StartDly = SPIDER_DLY_TIME;			  //赋值STM启动延时
	APP_SPIDER.InitOKFlag = FALSE;					  //清除STM初始化成功标志位
	APP_SPIDER.InitOverTimeCnt = MAIN_SPIDER_INIT_TIME;//赋值STM初始化时间
	HalTim4_StrPar.TIM_count = 0;						  //清零TIM4的计数值
	while(HalTim4_StrPar.TIM_count <= 500){
		if(HalTim4_StrPar.TIM_count > 300){
			SPIDER_TM_RESET_ENABLE();					//在0.3s到0.5s之间复位STM
			break;
		}
	}
	for(i = 0; i < 0xFF; i++);								//延时
	SPIDER_TM_RESET_DISABLE();							//结束复位
	for(i = 0; i < 0xFFFF; i++);							//延时
	while((APP_SPIDER.InitOverTimeCnt != 0 ) && (APP_SPIDER.InitOKFlag == FALSE )){
      if(HalTim4_StrPar.TIM_T1SFlag == TRUE){
			HalTim4_StrPar.TIM_T1SFlag = FALSE;		//清除1秒标志位
			APP_SPIDER.InitOverTimeCnt--;				//倒计时超时时间
			if(APP_SPIDER.StartDly != 0){
				APP_SPIDER.StartDly --;					//倒计时启动时间
#ifdef DEBUG_MCU_STATUS
				printf("\r\n【MCU Status】:%d\r\n",APP_SPIDER.StartDly);
#endif
				if(APP_SPIDER.StartDly == 0){
					APP_SPIDER.InitDealAddr = 1;			//设置处理spider初始化事件ID为1
				}
			}
		}
		/* 计时未到一秒:spider事件ID为0	不处理*/
		/* 启动延时结束:spider事件ID为1	读取模块形态,第一次读取不延时,之后读取延时200ms*/
		APP_spider_UsartDealForInit();
		APP_spider_uartDeal();
		APP_Spider_dealSendCmdToSpider();
		APP_spider_IEPIN_deal();
	}
	if(APP_SPIDER.InitOKFlag==TRUE){
		
#ifdef DEBUG_MCU_STATUS
		printf("\r\nSpider module init success!\r\n");
#endif

	}
	else{
		
#ifdef DEBUG_MCU_STATUS
		printf("\r\nSpider module init timeout!\r\n");
#endif
	}
}

void APP_spider_uartDeal(void){
	u8 i = 0;
	u8 crc1 = 0;
	u8 len1=3,len2=4,len3=5;
	// UART3  接收处理
	if(UART3.RxDataSucFlag==TRUE){
        
		// 处理数据
		crc1= ALG_CmdCrcCal(&UART3.RxBuff[1],(UART3.RxLeng+2));//计算校验值
                
		if(UART3.RxBuff[UART3.RxLeng+3]==crc1){
                  
			APP_SPIDER.UartRevOkFlag=TRUE;
#ifdef DEBUG_DDA_TO_MCU                     
            printf("【DDA to MCU】");
            for(int cnt=0;cnt<UART3.RxLeng+5;cnt++)
			{
				printf("%02X ",UART3.RxBuff[cnt]);
			}
			printf("\r\n");
#endif
                        
			// 判断应答结果
			if(UART3.RxBuff[len3]==0x80){
                          
				switch(UART3.RxBuff[len1]){
					case 0:break;
					case 1://配置类
						if(UART3.RxBuff[len2]==0x01){

						}
						else if(UART3.RxBuff[len2]==0x02){
							
						}
						break;
					case 2:  // 无线类
						
						break;
					case 3:  // 控制类
						if(UART3.RxBuff[len2]==0x01){

						}
						else if(UART3.RxBuff[len2]==0x02){

						}
						else if(UART3.RxBuff[len2]==0x03){

						}
						else if(UART3.RxBuff[len2]==0x04){

						}
						else if(UART3.RxBuff[len2]==0x11){

						}
						break;
					case 4: //  查询类
						if(UART3.RxBuff[len2]==0x01){
							// 模块状态
							Spider_IEPIN_StatusDeal(UART3.RxBuff);
						}
						else if(UART3.RxBuff[len2]==0x02){
							// 工作参数
						}
						else if(UART3.RxBuff[len2]==0x03){
							// 软件版本号
						}
						else if(UART3.RxBuff[len2]==0x04){
							// 模块序列号

						}
						else if(UART3.RxBuff[len2]==0x05){
							// 无线连接状态

						}
						else if(UART3.RxBuff[len2]==0x06){
							// 组网档案
							APP_SPIDER.Judge_Is_OnNetFlag = UART3.RxBuff[9];//组网标志
						}
						else if(UART3.RxBuff[len2]==0x07){
							// 无线参数

						}
						else if(UART3.RxBuff[len2]==0x08){
							// 系统时间

						}
						else if(UART3.RxBuff[len2]==0x09){

						}
						else if(UART3.RxBuff[len2]==0x0A){
							if(UART3.RxBuff[len3+1]==0x00)// 模块形态为一级
							{
								APP_SPIDER.MF_Flag=TRUE;
							}
						}
						else if(UART3.RxBuff[len2]==0x21){
							// 上行状态

						}
						else if(UART3.RxBuff[len2]==0x22){
							// 上次U包次数
                                                  
						}
						else if(UART3.RxBuff[len2]==0x23){
							// 上次M包次数
                                                  
						}
						else if(UART3.RxBuff[len2]==0x24){
							// 上次P包次数

						}
						else if(UART3.RxBuff[len2]==0x31){
                                                  
							APP_SPIDER.FastCmdFlag=TRUE;//about LED.
                                                        
							//下行快捷令
							switch(UART3.RxBuff[len3+1])
							{
								case 0:
									break;
								case 1:
									APP_flow_dealSendCmdToSpider();
									 break;
								case 2:
									break;
								case 3:
									break;
								case 4:
									break;
                                case 5:
									break;
								case 6:
									break;
								case 7:
									break;
								case 8:
									break;
								default:
									break;
							}							
						}
						else if(UART3.RxBuff[len2]==0x32){
							//下行U包
#ifdef DEBUG_DDA_TO_MCU                                               
                                                printf("\r\nReceived U Package:");
#endif
                                                for(i=0; i<UART3.RxLeng+5; i++)
                                                {
#ifdef DEBUG_DDA_TO_MCU                                               
                                                  printf("%02X ",UART3.RxBuff[i]);
#endif
												  APP_Screen.Tx_Data[i]=UART3.RxBuff[i];	//缓存U包数据
                                                }
#ifdef DEBUG_MCU_STATUS                                               
                                                printf("\r\n");
                                                printf("DDA TO Ultrasonic U数据长度：%02x\n",APP_Screen.Tx_Data[7]);
#endif											
                                                U_Packet_CntCache=UART3.RxBuff[9];	//缓存U包计数	
                                                //HAL_RS485_TxNbyte( (u8 *)(APP_Screen.Tx_Data+8),(u16)APP_Screen.Tx_Data[7],USER_USART2);	//485串口下发命令到ultrasonic
                                                APP_ULTRASONIC.GW_Down_Cmd_Flag=1;	//网关查询标志置位
                                            	//HAL_RS485_TxNbyte( (u8 *)(UART3.RxBuff+7),UART3.RxLeng-4,USER_USART2);	//485串口数据转发
												// PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDUPACK,g_temporary_buffer_for_screen.buffer,3);//生成U包命令
												// APP_SPIDER.DealSendCmdToSpiderAddr = 1;//标志有命令需要发送到spider
												//APP_Screen_dealSendCmdToSpider();	//DDA终端回执                                           	
						}
						else if(UART3.RxBuff[len2]==0x33){
							//下行M包
#ifdef DEBUG_DDA_TO_MCU                                               
                                                printf("\r\nReceived M Package:");
#endif
                                                for(i=0; i<UART3.RxLeng+5; i++)
                                                {
#ifdef DEBUG_DDA_TO_MCU                                               
                                                  printf("%02X ",UART3.RxBuff[i]);
#endif
												  APP_Screen.Tx_Data[i]=UART3.RxBuff[i];	//缓存M包数据
                                                }
#ifdef DEBUG_MCU_STATUS  
                                                printf("\r\n");
                                                printf("DDA TO Ultrasonic M数据长度：%02x\n",APP_Screen.Tx_Data[7]);
#endif
                                                //HAL_RS485_TxNbyte( (u8 *)(APP_Screen.Tx_Data+8),(u16)APP_Screen.Tx_Data[7],USER_USART2);	//485串口下发命令到ultrasonic
                                            	// HAL_RS485_TxNbyte( (u8 *)(UART3.RxBuff+7),UART3.RxLeng-4,USER_USART2);	//485串口数据转发
                                            	//APP_Screen_dealSendCmdToSpider();	//DDA终端回执  
						}
						else if(UART3.RxBuff[len2]==0x34){
							//下行P包次数
                                                  
						}
						break;
					case 5:  // 传输类
						if(UART3.RxBuff[len2]==0x01){
                                                  
						}
						else if(UART3.RxBuff[len2]==0x02){

						}
						else if(UART3.RxBuff[len2]==0x03){

						}
						else if(UART3.RxBuff[len2]==0x04){

						}
						else if(UART3.RxBuff[len2]==0x05){

						}
						else if(UART3.RxBuff[len2]==0x06){

						}
						else if(UART3.RxBuff[len2]==0x12){
                                                  
						}
						break;
					default:break;
				}
			}
		}
                
		// 重新开始接收
		UART3.RxDataSucFlag=FALSE;
		UART3.RxDealStep=1;
	}
  
}



/*
	注:函数放入到1s定时任务中
*/
void APP_spider_FastCmdLEDshake(void)
{
	if(APP_SPIDER.FastCmdLEDShakeFlag==TRUE)
	{
		//LED6_XOR();
	}
	else
	{
		if(APP_SPIDER.FastCmdFlag==TRUE)
		{
			//LED6_OFF();
		}
		
	}
	APP_SPIDER.FastCmdFlag=FALSE;
}




/*
	处理命令应答
*/
void APP_Spider_dealSendCmdToSpider(void){
	u8 Addr = APP_SPIDER.DealSendCmdToSpiderAddr;
	u8 s_tmp[5];
	switch(Addr){
		case 0:		//无操作
			break;
		case 1:
			if(HalTim4_StrPar.TIM_T200mSFlag==FALSE)break;
			HalTim4_StrPar.TIM_T200mSFlag=FALSE;
			HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);
				
				Addr++;
				APP_SPIDER.UartRevOkFlag=FALSE;
				APP_SPIDER.UartRevOverFlag=FALSE;
				APP_SPIDER.UartRevOverCnt=1000;//100
			break;
		case 2:
			if(APP_SPIDER.UartRevOkFlag==TRUE){
				APP_SPIDER.UartRevOkFlag=FALSE;
				Addr = 0;
		        APP_SPIDER.UartRevOverCnt=0;
			}
			if(APP_SPIDER.UartRevOverFlag==TRUE){
				APP_SPIDER.UartRevOverFlag=FALSE;
				Addr--;	
				HalTim4_StrPar.TIM_T200mSFlag=FALSE;
			}
			break;
                        
		//-----------切换Spider的工作模式为一级单终端/多终端模式
		case 5:
			if(HalTim4_StrPar.TIM_T200mSFlag==FALSE)break;		//200ms标志位为假，退出switch
			HalTim4_StrPar.TIM_T200mSFlag=FALSE;	//清除标志位
			s_tmp[0] = 0x01;						//单终端组网
			PRO_spider_BuildCMDForPar(CMD_TYPE_CTRL,CTRL_CMD_SWITCHWORKMODE,s_tmp,1);					//生成命令
		  	HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,SPIDER_USART);//发送命令
			Addr++;									//设置发送命令到spider模块的事件ID为6
			APP_SPIDER.UartRevOkFlag=FALSE;		//清除spider串口接收完成标志位
			APP_SPIDER.UartRevOverFlag=FALSE;		//清除spider串口接收超时标志位
			APP_SPIDER.UartRevOverCnt=500;			//设置spider串口接收超时时间为500s	
			break;
		case 6:
			if(APP_SPIDER.UartRevOkFlag==TRUE){
				APP_SPIDER.UartRevOkFlag=FALSE;
				Addr = 0;
		        APP_SPIDER.UartRevOverCnt=0;
				
			}
			if(APP_SPIDER.UartRevOverFlag==TRUE){
				APP_SPIDER.UartRevOverFlag=FALSE;
				Addr--;	
				HalTim4_StrPar.TIM_T200mSFlag=FALSE;
			}
			break;

		default :
			break;
		}
	 APP_SPIDER.DealSendCmdToSpiderAddr = Addr;
}

/*
	准备组网
	判断是否组网，若无则一直处于组网模式，若已组网，则将进行一次组网，不论超时还是组网成功，都会进入下面的程序
*/
void APP_Spider_ReadyOnNet(void){
	while(APP_SPIDER.Judge_ReadyOnNetFlag == FALSE){	//判断是否继续进入组网模式
		if(APP_SPIDER.Judge_SendOnNetCmdFlag == TRUE){	//判断是否发送组网命令
			APP_SPIDER.Judge_SendOnNetCmdFlag = FALSE;	//取消发送组网命令
			APP_SPIDER.DealSendCmdToSpiderAddr = 5;	//设置发送命令到spider模块的事件ID为5
		}
		APP_Spider_dealSendCmdToSpider();				//发送命令到spider模块的事件
		APP_spider_uartDeal();							//spider串口接收数据处理
		APP_spider_IEPIN_deal();
	}
}




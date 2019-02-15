#include "main.h"



#define ULTRASONIC_PRINT_SWITCH 0



ULTRAS_STRUCT  APP_ULTRASONIC;



const static u8 Ultrasonic_PollAddrCMD[3] = {0xFA, 0xF9, 0x03};



unsigned char Ultrasonic_CmdCrcCal(unsigned char *pdata, unsigned int len);



static Ultrasonic_Temporary_Buffer g_ultrasonic_temporary_buffer = {0};
static Ultrasonic_Control_Struct   g_ultrasonic_control_struct   = {0};



/*
	变量初始化 main函数中
*/
void APP_Ultrasonic_ValueInit(void)
{
	APP_ULTRASONIC.InitOKFlag = FALSE;
	APP_ULTRASONIC.StatuChaflag=FALSE;
	APP_ULTRASONIC.InitOverTimeCnt = 0;
	APP_ULTRASONIC.SendCmdOverTimeCnt = 0;
	APP_ULTRASONIC.InitDealAddr = 1;
	APP_ULTRASONIC.LocalAddr =  1;
	APP_ULTRASONIC.LedShakeFlag = 0;
	APP_ULTRASONIC.RevBuffCashe = 0;
	APP_ULTRASONIC.LedShakeTimCnt = 0;
	APP_ULTRASONIC.GW_Down_Cmd_Flag=0;
	APP_ULTRASONIC.TxCnt=0;
}


/*
	超声波初始化
*/
void APP_Ultrasonic_ConfigInit(void)
{
  APP_ULTRASONIC.InitOverTimeCnt = 30;   //设置超声波初始化超时时间为30s
  //Main_StrPar.Init_LED2_ShakeFlag = TRUE;//启动LED2闪烁
  
  while(APP_ULTRASONIC.InitOverTimeCnt !=0 && APP_ULTRASONIC.InitOKFlag == FALSE)//30s内一直查询超声波模块的地址,要么成功查询到了,要么超时了才退出
  {
    APP_Ultrasonic_UsartDealForInit();
    APP_Ultrasonic_UartRxDealforInit();
  }
  
  //Main_StrPar.Init_LED2_ShakeFlag = FALSE;//关闭LED2闪烁
  LED2_OFF();
}


void APP_Ultrasonic_UsartDealForInit(void)
{
	switch(APP_ULTRASONIC.InitDealAddr)
	{
		case 0:
			break;
		case 1:
			//发送查询地址命令
			HAL_USART_SendStringN((u8 *)Ultrasonic_PollAddrCMD,sizeof(Ultrasonic_PollAddrCMD),USER_USART2);
			APP_ULTRASONIC.InitDealAddr ++;
			APP_ULTRASONIC.SendCmdOverTimeCnt = 20;//4s
			break;
		case 2:
			if(APP_ULTRASONIC.SendCmdOverTimeCnt == 0)
			{
				if(APP_ULTRASONIC.InitOKFlag == TRUE)
				{
					APP_ULTRASONIC.InitDealAddr = 0;
				}
				else
				{
					APP_ULTRASONIC.InitDealAddr--;
				}
			}
			break;
		case 3:
			break;
		case 4:
			break;
		default:
			break;
	}
}


/*
	超声波初始化阶段串口接收处理
*/
void APP_Ultrasonic_UartRxDealforInit(void)
{
        //u8 cnt;
  
	if(UART2.RxDataSucFlag == TRUE)
	{
		//printf("UART2.RxDataSucFlag==TRUE\r\n");
		//printf("--%d--\r\n",UART2.RxCnt);
		/*for(cnt=0;cnt< UART2.RxCnt;cnt++)
		{
			printf("%02X ",UART2.RxBuff[cnt]);
		}
		printf("\r\n");*/
		
		if(UART2.RxCnt >= 4)
		{
			APP_ULTRASONIC.LocalAddr = UART2.RxBuff[3];//本机地址
			APP_ULTRASONIC.InitOKFlag = TRUE;
		}
		
		UART2.RxDealStep=1;//重新开始接收
		UART2.RxDataSucFlag = FALSE;
	}
}


/*
	超声波定时查询车位状态，在定时任务处理函数中
*/
void APP_Ultrasonic_FixedPollState(u8 CmdFlag)
{
  uint8_t i = 0;
  
  UART2.TxLeng = 0;
  UART2.TxBuff[UART2.TxLeng++] = 0x01;
  UART2.TxBuff[UART2.TxLeng++] = 0x04;
  UART2.TxBuff[UART2.TxLeng++] = 0x10;
  switch(CmdFlag)
  	{
  	   case 0://瞬间流量命令
	   	UART2.TxBuff[UART2.TxLeng++] = 0x10;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0x74;
        UART2.TxBuff[UART2.TxLeng++] = 0xCE;
		break;
		case 1://瞬间流量单位
	   	UART2.TxBuff[UART2.TxLeng++] = 0x20;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
        UART2.TxBuff[UART2.TxLeng++] = 0x34;
        UART2.TxBuff[UART2.TxLeng++] = 0xC0;
		break;
		case 2://瞬时流速
	   	UART2.TxBuff[UART2.TxLeng++] = 0x12;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0xD5;
        UART2.TxBuff[UART2.TxLeng++] = 0x0E;
		break;
  	   case 3://总量流量单位
	   	UART2.TxBuff[UART2.TxLeng++] = 0x21;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
        UART2.TxBuff[UART2.TxLeng++] = 0x65;
        UART2.TxBuff[UART2.TxLeng++] = 0x00;
		break;
	   case 4://报警状态检测
	   	UART2.TxBuff[UART2.TxLeng++] = 0x24;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
        UART2.TxBuff[UART2.TxLeng++] = 0x75;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
		break;  
	   case 5://累计流量整数部分
	   	UART2.TxBuff[UART2.TxLeng++] = 0x18;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0xF5;
        UART2.TxBuff[UART2.TxLeng++] = 0x0C;
		break;
	   case 6://累计流量浮点部分
	   	UART2.TxBuff[UART2.TxLeng++] = 0x1A;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0x54;
        UART2.TxBuff[UART2.TxLeng++] = 0xCC;
		break;	
	   case 7://总量流量单位
	   	UART2.TxBuff[UART2.TxLeng++] = 0x21;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
        UART2.TxBuff[UART2.TxLeng++] = 0x65;
        UART2.TxBuff[UART2.TxLeng++] = 0x00;
		break;
	   default:
	   	break;
  	}
	   	
  
#ifdef DEBUG_MCU_TO_SENSOR
printf("【MCU to Ultrasonic】:");
  for(i=0; i<8; i++)
  {
    printf("%02X ",UART2.TxBuff[i]);
  }
  printf("\r\n");
#endif
  
  HAL_RS485_TxNbyte(UART2.TxBuff,UART2.TxLeng,USER_USART2);
}


/*
	超声波工作模式下串口接收处理
*/
void APP_Ultrasonic_UartRxDealforWork(void)
{
	static u8 pkg_cnt=0;
	static u8 pkg_chacnt=0;
	u8 byte[4]={0};
	u8 s_tmp[17]={0};
	s_tmp[0]=0x01;					//状态包竞争时隙开关
	static   float f=0;
	float f_new=0;
	float dif=0.000;
        unsigned int aa;
    u8 cnt;
	u8 i;
        
	if(UART2.RxDataSucFlag==TRUE){		
	     if(UART2.RxDataFulFlag==TRUE){
#ifdef DEBUG_MCU_STATUS
            printf("【Receive Data From Ultrasonic】:");
			for(cnt=0;cnt< UART2.RxCnt;cnt++)
			{
				printf("%02X ",UART2.RxBuff[cnt]);
			}
			printf("\r\n");
#endif

				if(UART2.RxBuff[1] == 0x04){	                
				s_tmp[1]=REPORT_PKG_CLASS_STA;	//包类型——主动上报——状态变化
				
				s_tmp[3]=0x0C;					//数据长度
				s_tmp[4]=UART2.Rx_temp[3];//流量
				s_tmp[5]=UART2.Rx_temp[4];
				s_tmp[6]=0;
				s_tmp[7]=0;
				s_tmp[8]=UART2.Rx_temp[7];
				s_tmp[9]=UART2.Rx_temp[8];//流速
				s_tmp[10]=UART2.Rx_temp[9];
				s_tmp[11]=UART2.Rx_temp[10];
				s_tmp[12]=UART2.Rx_temp[11];
				s_tmp[13]=UART2.Rx_temp[12];
				s_tmp[14]=UART2.Rx_temp[13];//报警
				s_tmp[15]=UART2.Rx_temp[14];
#if 0
				byte[0]=s_tmp[7];
				byte[1]=s_tmp[6];
				byte[2]=s_tmp[5];
				byte[3]=s_tmp[4];
#endif   
                                
                                byte[0]=UART2.Rx_temp[6];
				byte[1]=UART2.Rx_temp[5];
				byte[2]=UART2.Rx_temp[4];
				byte[3]=UART2.Rx_temp[3];
				Byte_to_Float(&f_new,byte);
				dif=f_new-f;
                               aa  = f_new * 1000;
                                s_tmp[5] = aa / 256;
                                s_tmp[4] = aa % 256;
				 printf("【ppp】:");
                       //          printf("%d",aa);
				  printf("%fand%02X-%02X\n",f_new,s_tmp[5],s_tmp[4]);
				if((dif>0.1)||(dif<-0.1))
				APP_ULTRASONIC.StatuChaflag=TRUE;
				f=f_new;
				}	
			//预留数据错误的提示上报，待完善


			// if(UART2.RxCnt<6 && (UART2.RxBuff[1] != APP_ULTRASONIC.RevBuffCashe || APP_ULTRASONIC.GW_Down_Cmd_Flag ==1)){	//判断状态是否发生变化以及是否有网关查询指令
#if 0
#ifdef DEBUG_MCU_TO_DDA
            printf("【MCU to DDA U Package Data】:");
			for(cnt=0;cnt< g_ultrasonic_temporary_buffer.len;cnt++)
			{
				printf("%02X ",g_ultrasonic_temporary_buffer.buf[cnt]);
			}
			printf("\r\n");
#endif		
#endif
             if(HalTim4_StrPar.TIM_T1minFlag==TRUE){
			 	s_tmp[2]=pkg_cnt;				//包计数
			 	pkg_cnt++;
				memcpy((void *)g_ultrasonic_temporary_buffer.buf,(const void *)(s_tmp),17);
				g_ultrasonic_temporary_buffer.len = 17;	//length+data
				start_send_S_package();
				 HalTim4_StrPar.TIM_T1minFlag = FALSE;
				//APP_ULTRASONIC.StatuChaflag==FALSE;
             	}
			 if(APP_ULTRASONIC.StatuChaflag==TRUE){
			 	s_tmp[2]=pkg_chacnt;				//包计数
			 	pkg_chacnt++;
				memcpy((void *)g_ultrasonic_temporary_buffer.buf,(const void *)(s_tmp),17);
				g_ultrasonic_temporary_buffer.len = 17;	//length+data
				start_send_S_package();
				APP_ULTRASONIC.StatuChaflag=FALSE;
             	}
			 if(pkg_cnt==255)
			 	pkg_cnt=1;
			 if(pkg_chacnt==255)
			 	pkg_chacnt=1;
                
                
				UART2.RxDataFulFlag= FALSE;
	}
		  UART2.RxDataSucFlag = FALSE;
	     UART2.RxDealStep=1;//重新开始接收
		}
	
}


/*
超声波传感器中的crc校验
*/
unsigned char Ultrasonic_CmdCrcCal(unsigned char *pdata, unsigned int len)
{
  unsigned int i;
  unsigned char xorByte = pdata[0];
  
  for(i=1;i<len;i++)
  {
	xorByte ^= pdata[i];
  }
  return xorByte;
}


void start_send_U_package(void)//U包
{
  g_ultrasonic_control_struct.perform_status = ULTRASONIC_ACTIVE;
  g_ultrasonic_control_struct.timer = 0;
  
  PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDUPACK,g_ultrasonic_temporary_buffer.buf,g_ultrasonic_temporary_buffer.len);//发送U包
  APP_SPIDER.DealSendCmdToSpiderAddr = 1;
}
void start_send_S_package(void)//状态包
{
  g_ultrasonic_control_struct.perform_status = ULTRASONIC_ACTIVE;
  g_ultrasonic_control_struct.timer = 0;
  
  PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDSTATUS,g_ultrasonic_temporary_buffer.buf,g_ultrasonic_temporary_buffer.len);//发送状态包
  APP_SPIDER.DealSendCmdToSpiderAddr = 1;
}
#if 1
void start_send_Status_package(void)
{
   static u8 cnt=0;
   u8 s_tmp[20]={0};
   u8 i;  
   
   s_tmp[0]=0x01;// 竞争时隙
   for(i=1;i<17;i++)// 16 字节的数据
   {
      s_tmp[i]=cnt;
   }
   
   /*合成上行状态包*/
   PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDSTATUS,s_tmp,17);
   //修改发送spider数据的指向值
   APP_SPIDER.DealSendCmdToSpiderAddr = 1;
   cnt++;
}
#endif


void halt_send_U_package(void)
{
  memset((void *)&g_ultrasonic_temporary_buffer,0,sizeof(Ultrasonic_Temporary_Buffer));
  memset((void *)&g_ultrasonic_control_struct,0,sizeof(Ultrasonic_Control_Struct));
}

void ultrasonic_delay(void)
{
  if(g_ultrasonic_control_struct.perform_status == ULTRASONIC_ACTIVE)
  {
    g_ultrasonic_control_struct.timer++;
    if(g_ultrasonic_control_struct.timer > ULTRASONIC_RESEND_MAX_TIME)
    {
      start_send_U_package();
    }
  }
}


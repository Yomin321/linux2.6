#include "main.h"



#define ULTRASONIC_PRINT_SWITCH 0



ULTRAS_STRUCT  APP_ULTRASONIC;



const static u8 Ultrasonic_PollAddrCMD[3] = {0xFA, 0xF9, 0x03};



unsigned char Ultrasonic_CmdCrcCal(unsigned char *pdata, unsigned int len);



static Ultrasonic_Temporary_Buffer g_ultrasonic_temporary_buffer = {0};
static Ultrasonic_Control_Struct   g_ultrasonic_control_struct   = {0};



/*
	������ʼ�� main������
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
	��������ʼ��
*/
void APP_Ultrasonic_ConfigInit(void)
{
  APP_ULTRASONIC.InitOverTimeCnt = 30;   //���ó�������ʼ����ʱʱ��Ϊ30s
  //Main_StrPar.Init_LED2_ShakeFlag = TRUE;//����LED2��˸
  
  while(APP_ULTRASONIC.InitOverTimeCnt !=0 && APP_ULTRASONIC.InitOKFlag == FALSE)//30s��һֱ��ѯ������ģ��ĵ�ַ,Ҫô�ɹ���ѯ����,Ҫô��ʱ�˲��˳�
  {
    APP_Ultrasonic_UsartDealForInit();
    APP_Ultrasonic_UartRxDealforInit();
  }
  
  //Main_StrPar.Init_LED2_ShakeFlag = FALSE;//�ر�LED2��˸
  LED2_OFF();
}


void APP_Ultrasonic_UsartDealForInit(void)
{
	switch(APP_ULTRASONIC.InitDealAddr)
	{
		case 0:
			break;
		case 1:
			//���Ͳ�ѯ��ַ����
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
	��������ʼ���׶δ��ڽ��մ���
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
			APP_ULTRASONIC.LocalAddr = UART2.RxBuff[3];//������ַ
			APP_ULTRASONIC.InitOKFlag = TRUE;
		}
		
		UART2.RxDealStep=1;//���¿�ʼ����
		UART2.RxDataSucFlag = FALSE;
	}
}


/*
	��������ʱ��ѯ��λ״̬���ڶ�ʱ����������
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
  	   case 0://˲����������
	   	UART2.TxBuff[UART2.TxLeng++] = 0x10;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0x74;
        UART2.TxBuff[UART2.TxLeng++] = 0xCE;
		break;
		case 1://˲��������λ
	   	UART2.TxBuff[UART2.TxLeng++] = 0x20;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
        UART2.TxBuff[UART2.TxLeng++] = 0x34;
        UART2.TxBuff[UART2.TxLeng++] = 0xC0;
		break;
		case 2://˲ʱ����
	   	UART2.TxBuff[UART2.TxLeng++] = 0x12;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0xD5;
        UART2.TxBuff[UART2.TxLeng++] = 0x0E;
		break;
  	   case 3://����������λ
	   	UART2.TxBuff[UART2.TxLeng++] = 0x21;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
        UART2.TxBuff[UART2.TxLeng++] = 0x65;
        UART2.TxBuff[UART2.TxLeng++] = 0x00;
		break;
	   case 4://����״̬���
	   	UART2.TxBuff[UART2.TxLeng++] = 0x24;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
        UART2.TxBuff[UART2.TxLeng++] = 0x75;
        UART2.TxBuff[UART2.TxLeng++] = 0x01;
		break;  
	   case 5://�ۼ�������������
	   	UART2.TxBuff[UART2.TxLeng++] = 0x18;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0xF5;
        UART2.TxBuff[UART2.TxLeng++] = 0x0C;
		break;
	   case 6://�ۼ��������㲿��
	   	UART2.TxBuff[UART2.TxLeng++] = 0x1A;
		UART2.TxBuff[UART2.TxLeng++] = 0x00;
        UART2.TxBuff[UART2.TxLeng++] = 0x02;
        UART2.TxBuff[UART2.TxLeng++] = 0x54;
        UART2.TxBuff[UART2.TxLeng++] = 0xCC;
		break;	
	   case 7://����������λ
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
printf("��MCU to Ultrasonic��:");
  for(i=0; i<8; i++)
  {
    printf("%02X ",UART2.TxBuff[i]);
  }
  printf("\r\n");
#endif
  
  HAL_RS485_TxNbyte(UART2.TxBuff,UART2.TxLeng,USER_USART2);
}


/*
	����������ģʽ�´��ڽ��մ���
*/
void APP_Ultrasonic_UartRxDealforWork(void)
{
	static u8 pkg_cnt=0;
	static u8 pkg_chacnt=0;
	u8 byte[4]={0};
	u8 s_tmp[17]={0};
	s_tmp[0]=0x01;					//״̬������ʱ϶����
	static   float f=0;
	float f_new=0;
	float dif=0.000;
        unsigned int aa;
    u8 cnt;
	u8 i;
        
	if(UART2.RxDataSucFlag==TRUE){		
	     if(UART2.RxDataFulFlag==TRUE){
#ifdef DEBUG_MCU_STATUS
            printf("��Receive Data From Ultrasonic��:");
			for(cnt=0;cnt< UART2.RxCnt;cnt++)
			{
				printf("%02X ",UART2.RxBuff[cnt]);
			}
			printf("\r\n");
#endif

				if(UART2.RxBuff[1] == 0x04){	                
				s_tmp[1]=REPORT_PKG_CLASS_STA;	//�����͡��������ϱ�����״̬�仯
				
				s_tmp[3]=0x0C;					//���ݳ���
				s_tmp[4]=UART2.Rx_temp[3];//����
				s_tmp[5]=UART2.Rx_temp[4];
				s_tmp[6]=0;
				s_tmp[7]=0;
				s_tmp[8]=UART2.Rx_temp[7];
				s_tmp[9]=UART2.Rx_temp[8];//����
				s_tmp[10]=UART2.Rx_temp[9];
				s_tmp[11]=UART2.Rx_temp[10];
				s_tmp[12]=UART2.Rx_temp[11];
				s_tmp[13]=UART2.Rx_temp[12];
				s_tmp[14]=UART2.Rx_temp[13];//����
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
				 printf("��ppp��:");
                       //          printf("%d",aa);
				  printf("%fand%02X-%02X\n",f_new,s_tmp[5],s_tmp[4]);
				if((dif>0.1)||(dif<-0.1))
				APP_ULTRASONIC.StatuChaflag=TRUE;
				f=f_new;
				}	
			//Ԥ�����ݴ������ʾ�ϱ���������


			// if(UART2.RxCnt<6 && (UART2.RxBuff[1] != APP_ULTRASONIC.RevBuffCashe || APP_ULTRASONIC.GW_Down_Cmd_Flag ==1)){	//�ж�״̬�Ƿ����仯�Լ��Ƿ������ز�ѯָ��
#if 0
#ifdef DEBUG_MCU_TO_DDA
            printf("��MCU to DDA U Package Data��:");
			for(cnt=0;cnt< g_ultrasonic_temporary_buffer.len;cnt++)
			{
				printf("%02X ",g_ultrasonic_temporary_buffer.buf[cnt]);
			}
			printf("\r\n");
#endif		
#endif
             if(HalTim4_StrPar.TIM_T1minFlag==TRUE){
			 	s_tmp[2]=pkg_cnt;				//������
			 	pkg_cnt++;
				memcpy((void *)g_ultrasonic_temporary_buffer.buf,(const void *)(s_tmp),17);
				g_ultrasonic_temporary_buffer.len = 17;	//length+data
				start_send_S_package();
				 HalTim4_StrPar.TIM_T1minFlag = FALSE;
				//APP_ULTRASONIC.StatuChaflag==FALSE;
             	}
			 if(APP_ULTRASONIC.StatuChaflag==TRUE){
			 	s_tmp[2]=pkg_chacnt;				//������
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
	     UART2.RxDealStep=1;//���¿�ʼ����
		}
	
}


/*
�������������е�crcУ��
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


void start_send_U_package(void)//U��
{
  g_ultrasonic_control_struct.perform_status = ULTRASONIC_ACTIVE;
  g_ultrasonic_control_struct.timer = 0;
  
  PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDUPACK,g_ultrasonic_temporary_buffer.buf,g_ultrasonic_temporary_buffer.len);//����U��
  APP_SPIDER.DealSendCmdToSpiderAddr = 1;
}
void start_send_S_package(void)//״̬��
{
  g_ultrasonic_control_struct.perform_status = ULTRASONIC_ACTIVE;
  g_ultrasonic_control_struct.timer = 0;
  
  PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDSTATUS,g_ultrasonic_temporary_buffer.buf,g_ultrasonic_temporary_buffer.len);//����״̬��
  APP_SPIDER.DealSendCmdToSpiderAddr = 1;
}
#if 1
void start_send_Status_package(void)
{
   static u8 cnt=0;
   u8 s_tmp[20]={0};
   u8 i;  
   
   s_tmp[0]=0x01;// ����ʱ϶
   for(i=1;i<17;i++)// 16 �ֽڵ�����
   {
      s_tmp[i]=cnt;
   }
   
   /*�ϳ�����״̬��*/
   PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDSTATUS,s_tmp,17);
   //�޸ķ���spider���ݵ�ָ��ֵ
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


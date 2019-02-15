#include "APP_Screen.h"

Screen_Ack_Struct APP_Screen_Ack;
Screen_Struct	APP_Screen;
Flow_Struct     APP_Flow;


void APP_Screen_dealSendCmdToSpider(void){
	u8 i=0;
	APP_Screen_Ack.Data[0]=0x0A;	//服务质量
	APP_Screen_Ack.Data[1]=0x01;	//ultrasonic  ack length 	
	APP_Screen_Ack.Data[2]=0x80;	// screen ack status
	APP_Screen_Ack.Data[3]=0x00;
	PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDUPACK,APP_Screen_Ack.Data,Screen_Ack_Lenght);//生成U包命令
	HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,USER_USART3);	//mcu to spider
	APP_SPIDER.DealSendCmdToSpiderAddr = 1;//标志有命令需要发送到spider
#ifdef DEBUG_MCU_TO_DDA
    printf("\r\nUltrasonic_to_DDA ACK U Package:");
    for(i=0; i<APP_SPIDER.TxUsartLeng; i++)
    {
      printf("%02X ",APP_SPIDER.TxUsartData[i]);
    }
	printf("\r\n");
#endif
}
void APP_flow_dealSendCmdToSpider(void){
	u8 i=0;
	static u8 pack_cnt=0;
	APP_Flow.Rx_Data[0]=0x01;	
	APP_Flow.Rx_Data[1]=0x60;
	APP_Flow.Rx_Data[2]=pack_cnt;
	APP_Flow.Rx_Data[3]=0x09;
	APP_Flow.Rx_Data[4]=UART2.Rx_temp[15];
	APP_Flow.Rx_Data[5]=UART2.Rx_temp[16];
	APP_Flow.Rx_Data[6]=UART2.Rx_temp[17];
	APP_Flow.Rx_Data[7]=UART2.Rx_temp[18];
	APP_Flow.Rx_Data[8]=UART2.Rx_temp[19];
	APP_Flow.Rx_Data[9]=UART2.Rx_temp[20];
	APP_Flow.Rx_Data[10]=UART2.Rx_temp[21];
	APP_Flow.Rx_Data[11]=UART2.Rx_temp[22];
	APP_Flow.Rx_Data[12]=UART2.Rx_temp[24];
	pack_cnt++;
	PRO_spider_BuildCMDForPar(CMD_TYPE_TRFER,TRFER_CMD_SENDSTATUS,APP_Flow.Rx_Data,Flow_Ack_Lenght);//生成状态包命令
	HAL_USART_SendStringN((u8 *)APP_SPIDER.TxUsartData,APP_SPIDER.TxUsartLeng,USER_USART3);	//mcu to spider
	APP_SPIDER.DealSendCmdToSpiderAddr = 1;//标志有命令需要发送到spider
#ifdef DEBUG_MCU_TO_DDA
    printf("\r\nUltrasonic_to_DDA ACK U Package:");
    for(i=0; i<APP_SPIDER.TxUsartLeng; i++)
    {
      printf("%02X ",APP_SPIDER.TxUsartData[i]);
    }
	printf("\r\n");
#endif
}


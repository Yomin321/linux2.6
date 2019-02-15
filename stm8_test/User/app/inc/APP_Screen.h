#ifndef __APP_SCREEN_H
#define __APP_SCREEN_H

#include "main.h"
#include <string.h>

#define Screen_Ack_Lenght 13
#define Screen_Tx_Lenght 140
#define Flow_Rx_Lenght  20
#define Flow_Ack_Lenght 17



typedef struct{
	u8	Data[Screen_Ack_Lenght];		//Êý¾Ý
	u16	Status;							//×´Ì¬
}Screen_Ack_Struct;						//Screen to DDA receipt 

typedef struct{
	u8	Tx_Data[Screen_Tx_Lenght];		//×ª·¢»º´æ
	u16	Status;						//×´Ì¬
}Screen_Struct;						//to Screen

typedef struct{
	u8	Rx_Data[Flow_Rx_Lenght];		//×ª·¢»º´æ
	u16	Status;						//×´Ì¬
}Flow_Struct;


extern Screen_Ack_Struct APP_Screen_Ack;
extern Screen_Struct	APP_Screen;
extern Flow_Struct     APP_Flow;


void APP_Screen_dealSendCmdToSpider(void);
void APP_flow_dealSendCmdToSpider(void);


#endif

#ifndef __APP_CAMERA_H
#define __APP_CAMERA_H


#include "main.h"
#include <string.h>

#define PLATEMAXLENG 16
#define CAMERADEFAULTID 20

typedef struct{
	u8	LocaTion;			//停车位
	u8	PlateColor;			//车牌颜色
	u8	PlateLeng;			//车牌数据长度
 	u8	PlateNumber[PLATEMAXLENG];	//车牌号码
	u8	SpaceState;			//停车状态
	u16	CameraID;			//摄像头ID
	u8 DataSucFlag;		//数据保存成功标志位
}CAMERA_STRUCT;

extern CAMERA_STRUCT APP_CAMERA;

void APP_Camera_ValueInit(void);
void APP_Camera_uartDeal(void);
void APP_Camera_dealSendCmdToSpider(void);

#endif

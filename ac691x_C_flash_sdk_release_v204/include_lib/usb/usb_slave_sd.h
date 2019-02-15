/*******************************************************************************************
File Name: 	usb_slave_sd.h

Version: 	1.00

Discription SD Host 接口函数

Author: 	Caibingquan

Email:  	change.tsai@gmail.com

Date:		2013.04.27

Copyright (c) 2010 - 2012 JieLi. All Rights Reserved.
*******************************************************************************************/
#ifndef _USB_SLAVE_SD_H_
#define _USB_SLAVE_SD_H_

#include "typedef.h"

/*****************************
        Typedef
*****************************/
enum {
    SD_CARD_R = 1,
    SD_CARD_W,
};

typedef enum {
    OFFLINE = 0,                           ///<SD卡不在线
    UNACTIVE,                                ///<SD卡刚插入,未进行初始化
    ACTIVED,
} CardSta;


#endif  /*  _USB_SLAVE_SD_H_    */

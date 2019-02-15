/*
*********************************************************************************************************
*                                             BC51
*
*                                             CODE
*
*                          (c) Copyright 2015-2016, ZHUHAI JIELI
*                                           All Rights Reserved
*
* File : *
* By   : jamin.li
* DATE : 11/11/2015 build this file
*********************************************************************************************************
*/

#ifndef _LADC_H_
#define _LADC_H_

#include "typedef.h"
#include "cbuf/circular_buf.h"
#include "audio/ladc_api.h"

void ladc_reg_init(u16 sr);
void ladc_close(void);
void microphone_open(u8 mic_gain, u8 mic_gx2);
void microphone_close(void);

#endif

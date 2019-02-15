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
* junqian 20170329 add para inbuf_len,outbuf_len,kick_start_len,cbuf_len
*********************************************************************************************************
*/
#ifndef _SRC_API_H_
#define _SRC_API_H_

#include "hw_cpu.h"
#include "cpu.h"
#include "common/printf.h"
#include "typedef.h"

typedef struct {
    u8 nchannel;        //一次转换的通道个数，取舍范围(1 ~ 8)，最大支持8个通道
    u8 reserver[3];
    u16 in_rate;        ///输入采样率
    u16 out_rate;       ///输出采样率
    u16 in_chinc;       ///输入方向,多通道转换时，每通道数据的地址增量
    u16 in_spinc;       ///输入方向,同一通道后一数据相对前一数据的地址增量
    u16 out_chinc;      ///输出方向,多通道转换时，每通道数据的地址增量
    u16 out_spinc;      ///输出方向,同一通道后一数据相对前一数据的地址增量
    void (*output_cbk)(u8 *, u16, u8); ///一次转换完成后，输出中断会调用此函数用于接收输出数据，数据量大小由outbuf_len决定
} src_param_t;

typedef struct {
    u8 nchannel;  		///转换的通道个数
    u8 toggle;			///SRC开关
    u16 reserved;		///aligned
    u16 inbuf_len;		///SRC输入中断发生时，SRC模块一次从cbuf中输入的数据量。
    u16 outbuf_len;     ///SRC输出中断发生时，SRC模块一次输出的数据量。
    void (*isr_cb)(u8 *buf, u16 len, u8 flag); ///转换后数据输出回调，中断调用
    u16 *inbuf_addr;  	///转换输入物理buffer
    u16 *outbuf_addr; 	///转换输出物理buffer
    u32 *fltb_addr;  	///每个通道缓冲需要48bytes
} src_var_t;

unsigned int src_mem_query(int inbuf_len, int outbuf_len, int ch_max);
int src_init_api(src_param_t *parm, void *mem);
void src_exit_api(void);
u32 src_write_api(u8 *buf, u16 len);
void src_clear_api(void);
void src_config_api(src_param_t *parm);

#endif

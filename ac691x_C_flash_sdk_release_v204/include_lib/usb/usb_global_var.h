/*******************************************************************************************
File Name: 	USB_GLOBAL_VAR.c

Version: 	1.00

Discription USB 模块全局变量定义函数

Author:     huayue_hu

Email:      huayue_hu@zh-jieli.com

Date:		2014.10.30

Copyright (c) 2010 - 2012 JieLi. All Rights Reserved.
*******************************************************************************************/
#ifndef __USB_GLOBAL_VRA_H__
#define __USB_GLOBAL_VRA_H__

#include "string.h"
#include "usb_host_scsi.h"
#include "usb_slave_ctl.h"
#include "usb_slave_scsi.h"
#include "usb_slave_api.h"
#include "usb_host_api.h"
#include "uart.h"
extern u32 os_time_get(void);

/*USB模块打印*/
//#define USB_SLAVE_PUTS
#ifdef USB_SLAVE_PUTS
#define susb_puts           puts
#define susb_printf         printf
#define susb_printf_buf     printf_buf
#define susb_put_u32hex0    put_u32hex
#else
#define susb_puts(...)
#define susb_printf(...)
#define susb_printf_buf(...)
#define susb_put_u32hex0(...)
#endif


//#define USB_HOST_PUTS
#ifdef USB_HOST_PUTS
#include "uart.h"
#define husb_puts           puts
#define husb_printf         printf
#define husb_put_u8hex      put_u8hex
#define husb_printf_buf     printf_buf
#define husb_put_u32hex0    put_u32hex
#else
#define husb_puts(...)
#define husb_printf(...)
#define husb_put_u8hex(...)
#define husb_printf_buf(...)
#define husb_put_u32hex0(...)
#endif

/*USB SLAVE AUDIO使能控制*/
#define USB_SLAVE_SPEAKER_DAC   1
#define USB_SLAVE_MIC_ADC       1

#define USER_PC_DATAPROTECT		1

//USB IO define
#define USB_DP_OUT(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(0)) | ((x & 0x1)<<0))
#define USB_DM_OUT(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(1)) | ((x & 0x1)<<1))
#define USB_DP_DIR(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(2)) | ((x & 0x1)<<2))
#define USB_DM_DIR(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(3)) | ((x & 0x1)<<3))
#define USB_DP_PD(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(4)) | ((x & 0x1)<<4))
#define USB_DM_PD(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(5)) | ((x & 0x1)<<5))
#define USB_DP_PU(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(6)) | ((x & 0x1)<<6))
#define USB_DM_PU(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(7)) | ((x & 0x1)<<7))
#define USB_DP_DIE(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(9)) | ((x & 0x1)<<9))
#define USB_DM_DIE(x)		JL_USB->IO_CON0 = ((JL_USB->IO_CON0 & ~BIT(10)) | ((x & 0x1)<<10))

#define USB_DP_IN()     	(JL_USB->IO_CON1&BIT(0))
#define USB_DM_IN()     	(JL_USB->IO_CON1&BIT(1))

/*强制声明，避免warning*/
void put_u8hex(u8 dat);
void set_ie(u32 index);
void clear_ie(u32 index);
int printf(const char *format, ...);
void printf_buf(u8 *buf, u32 len);

typedef struct __USB_ONLINE_VAR {
    u32 usb_last_io; //
    volatile u8  online_flag;
    u8  online_cnt;
    u8  pcin_double_check;
    u8  align;               //保留对齐
} USB_ONLINE_VAR;

typedef struct __HUSB_READ_CTL {
    u32 read_page_size;
    u32 lg_secperlba;
} HUSB_READ_CTL;

enum {
    HUSB_READ_DATA_OPT  = 0,
    HUSB_OTHER_OPT,
};

/*外部变量声明*/
///COMMON
extern USB_ONLINE_VAR g_usb_detect_var;
///USB HOST
extern sUSB_BULK_ONLY USB_Bulk_only; //bulk_only cbw/csw buffer
extern sUSB_VAR *g_usb_var_p;        //全局指针(指向api接口层变量地址)
///USB DEVICE
extern USB_SLAVE_VAR *g_susb_var;
extern u8 *g_susb_dmabuf_p;
// extern OS_MUTEX mass_mutex;
extern HUSB_READ_CTL g_husb_read_ctl;

/*for usb detect*/
#define GET_USB_ONLINE_STATUS  (g_usb_detect_var.online_flag) //获取USB模块总在线情况
#define GET_HUSB_ONLINE_STATUS (g_usb_detect_var.online_flag & BIT(0)) //实时获取U盘在线情况
#define GET_SUSB_ONLINE_STATUS (g_usb_detect_var.online_flag & BIT(1)) //实时获取从机在线情况
#define GET_PC_ONLINE          (g_usb_detect_var.online_flag & BIT(2)) //PC状态机

/*函数声明*/
void usb_mem_pfree(void *p, u32 size);

#endif/*__USB_GLOBAL_VRA_H__*/


/*******************************************************************************************
File Name: usb_host_internal_api.h

Version: 1.00

Discription

Author: huayue_hu

Email:  huayue.hu.jl@gmail.com

Date:2012.02.06

Copyright:(c) 2012 , All Rights Reserved.
*******************************************************************************************/
#ifndef __USB_HOST_INTERNAL_API_H__
#define __USB_HOST_INTERNAL_API_H__

#include "typedef.h"
#include "usb_global_var.h"

/**************************************
		EP0
**************************************/
#define EP0_FIFO_LEN		  0x08

#define M_EP0_IDLE			    0
#define M_EP0_TRANSFER			1
#define	M_EP0_RECEIVE			2
//----------------------------------
// Standard Chapter 9 definition
//----------------------------------
#define STD_REQUEST			  	0x00
#define CLASS_REQUEST		  	0x01
#define VENDOR_REQUEST    		0x02
#define RECIPIENT_DEV		  	0x00
#define RECIPIENT_IFC		  	0x01
#define RECIPIENT_ENP		  	0x02

///////标准USB 设备请求
#define GET_STATUS      	0x00
#define CLEAR_FEATURE   	0x01
#define SET_FEATURE    		0x03
#define SET_ADDRESS    		0x05
#define GET_DESCRIPTOR 		0x06
#define SET_DESCRIPTOR 		0x07
#define GET_CONFIG     		0x08
#define SET_CONFIG    		0x09
#define GET_INTERFACE  		0x0A
#define SET_INTERFACE  		0x0B
#define SYNCH_FRAME     	0x0C

///////标准USB 描述符的类型值
#define DEVICE          	0x01
#define CONFIGURATION   	0x02
#define STRING          	0x03
#define INTERFACE       	0x04
#define ENDPOINT        	0x05

//////令牌包类型
#define	IN		0x1
#define OUT		0x2
#define SETUP	0x3
#define SOF		0x4
#define PRE		0x5
#define SPLIT	0x6
#define PING	0x7

#define STDCLASS        	0x00
#define HIDCLASS        	0x03
#define HUBCLASS          0x09

//----------------------------------
// Class Descriptor for HID
//----------------------------------
#define GET_REPORT      	0x01
#define GET_IDLE        	0x02
#define GET_PROTOCOL    	0x03

#define SET_REPORT      	0x09
#define SET_IDLE        	0x0A
#define SET_PROTOCOL    	0x0B

#define HID_DEV         	0x21
#define HID_REPORT      	0x22
#define HID_PHYSICAL    	0x23

//#define INPUT               0x01
//#define OUPUT     	    	0x02
//#define FEATURE    	   		0x03

#define NONE              0x00
#define KEYBOARD        	0x01
#define MOUSE             0x02


#define USB_ENUMERATED			        0
#define USB_GET_MAXLUN_STALL			1
#define USB_ENUMERTED_FAIL 	            2
#define USB_SUSPENDED					3

#define USB_AUDIO_ENUMERATED			4
#define USB_HID_ENUMERATED			    5

enum {
    USB_MSD = 0,
    USB_AUDIO,
    USB_HID,
};

/*设备操作状态*/
enum {
    DEV_IDLE = 0,
    DEV_READING,
    DEV_WRITTING,
};

/*USB在线状态*/
typedef enum __USB_ONLINE_STATUS {
    NULL_USB = 0,
    USB_HOST_ON = 1,
    USB_HOST_OFF = 2,
    USB_SLAVE_ON = 3,
    USB_SLAVE_OFF = 4,
} USB_ONLINE_STATUS;

USB_ONLINE_STATUS usb_online(u32 cnt);
void usb_online_status_save(USB_ONLINE_STATUS cur_status);
USB_ONLINE_STATUS usb_host_status_set(void);
u8 usb_is_online(void);
u32 usb_online_check(u32 cnt);
u32 usb_mult_online_check(u32 cnt);
u32 usb_get_capacity(void);
void usb_suspend(void);
s32 usb_host_mount(u8 retry, u16 timeout, u8 reset_delay, u8 test_delay, void *dma_buf);
s32 usb_host_unmount(void);
u8 usb_host_read(void *buf, u32 lba, u8 *dma_buf);
u8 usb_host_write(void *buf, u32 lba, u8 *dma_buf);
u8 usbh_msd_enum(void *dma_buf, u8 delay);
void  usb_host_status(u32 status);
void set_usb_io(void);
u8 usb_read_remain(void);
s32 usb_get_sector_unit(void);

#endif/*__USB_HOST_INTERNAL_API_H__*/

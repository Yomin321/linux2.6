/*******************************************************************************************
File Name: USB_Host_SCSI.h

Version: 1.00

Discription

Author: Caibingquan

Email:  change.tsai@gmail.com

Date:2012.02.06

Copyright (c) 2010 - 2012 JieLi. All Rights Reserved.
*******************************************************************************************/
#ifndef _USB_HOST_SCSI_H_
#define _USB_HOST_SCSI_H_

#include "usb_host_internal_api.h"
#include "usb_host_api.h"
#include "usb_slave_api.h"




u8 usb_ufi_read10(u8 offset, void *dma_buf);
u8 usb_ufi_write10(void *dma_buf);
u8 usb_bulk_only_send(void *buf, u16 len);
u8 usb_bulk_only_receive(void *buf, u16 len, u8 mode);
u8 usb_bulk_only_transfer(void *buf, u16 len, u8 offset, u8 mode);
tu8 usb_bulk_only_transfer_repeat(void *buf, tu16 len, tu8 offset);
tu8 USB_Host_Mass_Storage_enum(void *buf, tu8 test_delay);
void CBW_init(void);
u8 get_max_LUN(void *buf);
u8 USB_UFI_Inquiry(void *buf);
u8 USB_UFI_Test_unit_ready(void *buf);
u8 USB_UFI_Request_sense(void *buf);
u8 USB_UFI_Ready_capacity(void *dma_buf);

//#define Inquiry_data        ((INQUIRY_DATA *)g_usb_var_p->bulk_only_trans.pBuf)
//#define Read_Capacity_data  ((READ_CAPACITY_DATA *)g_usb_var_p->bulk_only_trans.pBuf)
//#define Request_Sense_data  ((REQUEST_SENSE_DATA *)g_usb_var_p->bulk_only_trans.pBuf)
#define Inquiry_data        ((INQUIRY_DATA *)dma_buf)
#define Read_Capacity_data  ((READ_CAPACITY_DATA *)dma_buf)
#define Request_Sense_data  ((REQUEST_SENSE_DATA *)dma_buf)

u16 usbh_msd_parser(void *ptr);

#endif  /*  _USB_HOST_SCSI_H_   */

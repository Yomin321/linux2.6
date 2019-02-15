/*******************************************************************************************
File Name: USB_Host_Reg.h

Version: 1.00

Discription

Author: Caibingquan

Email:  change.tsai@gmail.com

Date:2012.02.06

Copyright:(c) 2012 , All Rights Reserved.
*******************************************************************************************/
#ifndef _USB_HOST_REG_H_
#define _USB_HOST_REG_H_

#include "typedef.h"

/*****MUSB SFR BitMap******/

/*CSR0 host mode*/
#define CSR0H_RxPktRdy           0x01
#define CSR0H_TxPktRdy           0x02
#define CSR0H_RxStall            0x04
#define CSR0H_SetupPkt           0x08
#define CSR0H_Error              0x10
#define CSR0H_ReqPkt             0x20
#define CSR0H_StatusPkt          0x40

/*TXCSR1 host mode*/
#define TXCSR1H_TxPktRdy          0x01
#define TXCSR1H_FIFONotEmpty      0x02
#define TXCSR1H_Error             0x04
#define TXCSR1H_FlushFIFO         0x08
#define TXCSR1H_RxStall           0x20
#define TXCSR1H_ClrDataTog        0x40


/*RXCSR1 host mode*/
#define RXCSR1H_RxPktRdy          0x01
#define RXCSR1H_FIFOFull          0x02
#define RXCSR1H_Error             0x04
#define RXCSR1H_DataError         0x08
#define RXCSR1H_FlushFIFO         0x10
#define RXCSR1H_ReqPkt            0x20
#define RXCSR1H_RxStall           0x40
#define RXCSR1H_ClrDataTog        0x80


// usb phy 寄存器定义
#define     FADDR_REG               0x00
#define     POWER_REG               0x01
#define     INTRTX1_REG             0x02
#define     INTRTX2_REG             0x03
#define     INTRRX1_REG             0x04
#define     INTRRX2_REG             0x05
#define     USBINTR_REG             0x06
#define     INTRTX1E_REG            0x07
#define     INTRTX2E_REG            0x08
#define     INTRRX1E_REG            0x09
#define     INTRRX2E_REG            0x0A
#define     USBINTEN_REG            0x0B
#define     FRAMENUM1_REG           0x0C
#define     FRAMENUM2_REG           0x0D
#define     INDEX_REG               0x0E
#define     DEVCTL_REG              0x0F

#define     MAXP_TX_REG             0x10
#define     CSR0_REG                0x11
#define     TXCSR1_REG              0x11
#define     TXCSR2_REG              0x12
#define     MAXP_RX_REG             0x13
#define     RXCSR1_REG              0x14
#define     RXCSR2_REG              0x15
#define     COUNT0_REG              0x16
#define     RXCOUNT1_REG            0x16
#define     RXCOUNT2_REG            0x17
#define     TXTYPE_REG              0x18
#define     TXINTERVAL_REG          0x19
#define     RXTYPE_REG              0x1A
#define     RXINTERVAL_REG          0x1B

#define     TX_FIFO1                0x1C
#define     TX_FIFO2                0x1D
#define     RX_FIFO1                0x1E
#define     RX_FIFO2                0x1F

#define     EP0_FIFO                0x20
#define     EP1_FIFO                0x21
#define     EP2_FIFO                0x22
#define     EP3_FIFO                0x23
#define     EP4_FIFO                0x24

/*****************************
        Global Variables
*****************************/



/*****************************
        Function Declare
*****************************/
// void write_usb_reg(u8 addr, u8 value);
// u8 read_usb_reg(u8 addr);
void usb_host_init(void);
void usb_host_reset(void);
void usb_host_reset1(void);
void USB_DELAY_10MS(u32 cnt);

#endif  /*  _USB_HOST_REG_H_  */

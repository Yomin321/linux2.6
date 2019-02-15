#ifndef	_USB_SLAVE_REG_H_
#define _USB_SLAVE_REG_H_

#include "typedef.h"


/*****MUSB SFR BitMap******/
/*CSR0 peripheral mode*/
#define CSR0P_RxPktRdy           0x01
#define CSR0P_TxPktRdy           0x02
#define CSR0P_SendtStall         0x04
#define CSR0P_DataEnd            0x08
#define CSR0P_SetupEnd           0x10
#define CSR0P_SentStall          0x20
#define CSR0P_ClrRxPktRdy        0x40
#define CSR0P_ClrSetupEnd        0x80


/*TXCSR1 peripheral mode*/
#define TXCSR1P_TxPktRdy          0x01
#define TXCSR1P_FIFONotEmpty      0x02
#define TXCSR1P_UnderRun          0x04
#define TXCSR1P_FlushFIFO         0x08
#define TXCSR1P_SendStall         0x10
#define TXCSR1P_SentStall         0x20
#define TXCSR1P_ClrDataTog        0x40


/*RXCSR1 peripheral mode*/
#define RXCSR1P_RxPktRdy          0x01
#define RXCSR1P_FIFOFull          0x02
#define RXCSR1P_OverRun           0x04
#define RXCSR1P_DataError         0x08
#define RXCSR1P_FlushFIFO         0x10
#define RXCSR1P_SendStall         0x20
#define RXCSR1P_SentStall         0x40
#define RXCSR1P_ClrDataTog        0x80



void USB_Slave_reg_init(void);
void USB_Slave_ep0_init(void);


#endif	/*	_USB_SLAVE_REG_H_	*/

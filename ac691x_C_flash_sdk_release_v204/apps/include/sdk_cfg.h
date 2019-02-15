/*********************************************************************************************
    *   Filename        : sdk_cfg.h

    *   Description     : Config for Sound Box Case

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-12-01 15:36

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef _SDK_CONFIG_
#define _SDK_CONFIG_

#include "ac691x_cfg/ac691x_sdk_cfg.h"

// *INDENT-OFF*
/********************************************************************************/
/*
 *          --------调试类配置
 */
#define __DEBUG
#ifdef __DEBUG
  #define otp_printf		            printf
#else
  #define otp_printf(...)
#endif



#define UART0   0
#define UART1   1
#define UART2   2

#ifdef __DEBUG
  #define DEBUG_UART   UART0 /*select uart port*/
#else
  #define DEBUG_UART   0xFF  /*uart invalid*/
#endif

#if (DEBUG_UART == UART0)
  //#define UART_TXPA5_RXPA6
  #define UART_TXPB5_RXPA0
  //#define UART_TXPC2_RXPC3
  //#define UART_TXPA7_RXPA8
  //#define UART0_OUTPUT_CHAL
#endif

#if (DEBUG_UART == UART1)
  /* #define UART_TXPB0_RXPB1 */
  //#define UART_TXPA12_RXPC1
  //#define UART_TXPA1_RXPA2
  #define UART_USBP_USBM
  //#define UART1_OUTPUT_CHAL
#endif

#if (DEBUG_UART == UART2)
// #define UART_TXPA3_RXPA4
  //#define UART_TXPA9_RXPA10
  //#define UART_TXPB2_RXPB3
  #define UART_TXPC4_RXPC5
  //#define UART2_OUTPUT_CHAL
#endif


#define UART_BAUD_RAE 460800
#endif



/********************************************************************************/
/*
 *          --------用户自定义宏配置
 */
#define USER_XXXX     0


///记忆存储信息,EEPROM_EN,NV_MEM_EN,两个同时使能时，会自动检测，优先使用eeprom
#define EEPROM_EN        0 //使用 eeprom存储信息，有些芯片自带eeprom,use PA12\PA11
#define VM_MEM_EN        1 //使用 vm_flash存储信息
#define NV_MEM_EN        1 //使用 rtc_ram存储信息，掉电不保存

// *INDENT-ON*

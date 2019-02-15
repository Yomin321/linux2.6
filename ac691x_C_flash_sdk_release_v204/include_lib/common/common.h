/*--------------------------------------------------------------------------*/
/**@file     common.h
   @brief
   @details
   @author
   @date    2011-3-7
   @note    CD003
*/
/*----------------------------------------------------------------------------*/
#ifndef _COMMON_
#define _COMMON_

#include "typedef.h"

extern volatile u16 delay_cnt;

struct mem_stats {
    const char *name;
    unsigned int avail;
    unsigned int used;
    unsigned int max;
    unsigned long err;
    unsigned long illegal;
};

extern struct mem_stats sys_mem_sta;

#define GET_SYS_MEM_STAT() malloc_stats()


void delay(u32  t);
u32 dev_delay_10ms();
void dev_delay_10ms_count();
void delay_2ms_count();
void delay_2ms(u32 delay_time);


#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */


extern u16 CRC16(const void *ptr, u32 len);

//boot_arg_define
typedef enum {
    BT_XOSC_TYPE = 1,
    RTC_XOSC_TYPE,
} ARG_OSC_TYPE;

typedef enum {
    SDDEVICE_NOR_FLASH 	= 0x00,
    SDDEVICE_SDCARD 	= 0x01,
    SDDEVICE_NAND_FLASH = 0x02,
} ARG_DEV_TYPE;

typedef enum {
    SPI0_PORTD_A = 0x00,
    SPI0_PORTB_B = 0x01,
} ARG_SPI_PORT;


typedef enum {
    RUN_UBOOT_NORMAL = 0,
    RUN_UBOOT_FROM_FIX,
} ARG_BOOT_METHOD;


//boot_arg
typedef struct _boot_arg {
    u8 osc_type;		//ARG_OSC_TYPE
    u8 dev_type;		//ARG_DEV_TYPE
    u8 spi_port;		//ARG_SPI_PORT
    u8 osc_freq;		//
    u32 file_size;		//
    u32 boot_method;	//ARG_BOOT_METHOD
} BOOT_ARG;

typedef struct _otp_arg {
    u8 lvd         ;
    u8 chargeA     ;
    u8 chargeV     ;
} OTP_ARG;

/*
* 下面的结构体用于定义一些系统不同功能之间的变量判断，
* 用于减少系统模块之间的耦合性
* */
typedef struct sys_global {
    u8 fast_test_flag;      /*1A:enter fast test*/
    u8 going_to_pwr_off;    /*enter pwr_off flag,maybe play some tone */
    u8 mask_task_switch_flag; /*when a phone call ,it can't change mode */
    volatile u8 sys_halfsec;   //timer use
    volatile u16 t0_cnt1;       //timer use
    volatile u16 t2_cnt1;       //timer use
} system_global_t;

extern system_global_t sys_global_value;

#endif

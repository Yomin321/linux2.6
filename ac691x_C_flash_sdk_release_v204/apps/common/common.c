/*--------------------------------------------------------------------------*/
/**@file     common.c
   @brief    常用公共函数
   @details
   @author
   @date    2011-3-7
   @note    CD003
*/
/*----------------------------------------------------------------------------*/
#include "typedef.h"
#include "common/common.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif

system_global_t sys_global_value = {
    .fast_test_flag = 0,
    .going_to_pwr_off = 0,
    .mask_task_switch_flag = 0,
};



void delay(u32 d)sec(.poweroff_text);
void delay(u32 d)
{
    while (d--) {
        __builtin_pi32_nop();
    }
}

/*
***********************************************************************************
*					DELAY_2MS_COUNT
*
*Description: This function is called by timer_isr
*
*Argument(s): none
*
*Returns	: none
*
*Note(s)	:
***********************************************************************************
*/
volatile u32 delay2ms_cnt = 0;
void delay_2ms_count()
{
    if (delay2ms_cnt) {
        delay2ms_cnt--;
    }
}
volatile u32 dev_delay10ms_cnt = 200;
void dev_delay_10ms_count()
{
    if (dev_delay10ms_cnt) {
        dev_delay10ms_cnt--;
    }
}

/*
***********************************************************************************
*					DELAY_2MS
*
*Description: This function is called by application to block delay
*
*Argument(s): delay_time	wait time = delay_time * 2ms
*
*Returns	: none
*
*Note(s)	: 1)call this function will pend process until delay time decrease to 0
***********************************************************************************
*/
void delay_2ms(u32 delay_time)
{
    delay2ms_cnt = delay_time;
    while (delay2ms_cnt);
}

u32 dev_delay_10ms()
{
    return dev_delay10ms_cnt;
}



//*************************stack detect function*************************//
#include "sdk_cfg.h"
/* #ifdef __DEBUG */
#if 0

#include "uart.h"

extern u32 STACK_START[];
extern u32 STACK_END[];

#define STACK_SHIFT     0x100
#define STACK_MAGIC     0x55555555

#define STACK_PROTECT_EN    0
extern void cpu_protect_ram(u32 start, u32 end, u8 range);
extern void prp_protect_ram(u32 begin, u32 end, u8 range);

void stack_detect_init(void)
{
    memset((void *)STACK_START, STACK_MAGIC, ((u32)STACK_END - (u32)STACK_START - STACK_SHIFT));
    memset((void *)((u32)STACK_START), 0xAA, 0x10);

    memset((void *)((u32)STACK_END - 0x20), STACK_MAGIC, 0x20);

    /* cpu_protect_ram((u32)STACK_START, (u32)STACK_START + 0x40, 1); */
    /* prp_protect_ram((u32)STACK_START, (u32)STACK_START + 0x40, 1); */
}

static int word_cmp(int *p, int ch, u32 num)
{
    int ret = num;
    while (num-- > 0) {
        if (*p++ != ch) {
            return ret - num;
        }
    }
    return 0;
}
u8 do_one_time = 1;
void stack_detect_api(void)
{
    /* printf("stack_start = 0x%x\n", STACK_START); */
    /* printf("stack_end = 0x%x\n", STACK_END); */
    /* printf_buf((void *)STACK_START, ((u32)STACK_END - (u32)STACK_START)); */

    int ret = 0;

    //usp check
    ret = word_cmp((void *)(STACK_START + 0x10), STACK_MAGIC, ((u32)STACK_END - (u32)STACK_START - 0x10) / sizeof(int));
    ret *= sizeof(int);     //word to byte
    /* if (ret < 0x100) {  //0x100 byte */
    if (1) {  //0x100 byte
        printf("warning: stack_reath = 0x%x\n", ret);
        if (do_one_time) {
            do_one_time = 0;
            put_buf((void *)(STACK_START), 0x200);
        }
    }

    //ssp check
    ret = word_cmp((void *)((u32)STACK_END - 0x20), STACK_MAGIC, 0x10 / sizeof(int));
    if (ret > 0) {
        printf("warning: sys_stack err = 0x%x\n", ret);
        printf_buf((void *)((u32)STACK_END - 0x20), 0x20);
    }

    /* extern void printf_buf(u8 *buf, u32 len); */
    /* printf_buf((void *)STACK_START, (ret * 4) + 32); */
}


#else
void stack_detect_init(void) {}
#endif


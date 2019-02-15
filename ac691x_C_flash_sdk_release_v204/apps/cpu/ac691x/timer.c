/***********************************Jieli tech************************************************
  File : timer.c
  By   : Juntham
  date : 2014-07-04 11:21
********************************************************************************************/
#include "sdk_cfg.h"
#include "timer.h"
#include "key_drv/key.h"
#include "clock_api.h"
/* #include "sys_detect.h" */
#include "sys_timer.h"
#include "led/led.h"
#include "key_drv/key_drv_ad.h"
#include "key_drv/key_drv_ir.h"
#include "irq_api.h"
#include "clock.h"
#include "rotate_dec.h"
#include "dac.h"
#include "msg.h"
#include "power_manage_api.h"
#include "power/charge.h"
#include "power/power.h"
#include "adc_api.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".timer_app_bss")
#pragma data_seg(	".timer_app_data")
#pragma const_seg(	".timer_app_const")
#pragma code_seg(	".timer_app_code")
#endif

void (*timer_callback_fun)(void);
extern void sys_timer_schedule();
extern void delay_2ms_count();
extern void dac_fade(void *ptr);
extern void key_scan(void *ptr);
extern void scan_capkey(void);
extern void adc_scan(void *param);
extern void led_cnt(u32 cnt);
extern void led_scan(void);
extern void dev_detect_fun(void *priv);
extern void ir_timeout(void);
extern u16 set_msg_power_on_cnt(u8 mode, u16 pwr_on_cnt);
extern void pc_check_api(void);
extern void aux_detect(void);



#define SOURCE_CLK	1	//0:LSB		1:OSC

#if SOURCE_CLK
#define TIMER_CLK        IOSC_CLK
#else
#define TIMER_CLK        LSB_CLK
#endif



#define MAX_TIME_CNT 0x7fff
#define MIN_TIME_CNT 0x0100

static const u16 timer_div[] = {
    /*0000*/    1,
    /*0001*/    4,
    /*0010*/    16,
    /*0011*/    64,
    /*0100*/    2,
    /*0101*/    8,
    /*0110*/    32,
    /*0111*/    128,
    /*1000*/    256,
    /*1001*/    4 * 256,
    /*1010*/    16 * 256,
    /*1011*/    64 * 256,
    /*1100*/    2 * 256,
    /*1101*/    8 * 256,
    /*1110*/    32 * 256,
    /*1111*/    128 * 256,
};

/*
中断用到的变量可以都定义到结构体，
然后通过接口获取，方便管理
*/
struct sys_global_var_str {
    volatile u8 sys_halfsec;
    volatile u16 t0_cnt1;
};
struct sys_global_var_str g_var;

u32 os_time_get(void)
{
    return get_jiffies(0, 0);
}
/*----------------------------------------------------------------------------*/
/**@brief  get halfsec flag
   @param
   @return
   @note

 */
/*----------------------------------------------------------------------------*/
u8 get_sys_halfsec(void)
{
    return g_var.sys_halfsec;
}

/*----------------------------------------------------------------------------*/
/**@brief  get 2ms's count
   @param
   @return counkt
   @note

 */
/*----------------------------------------------------------------------------*/
void timer0_isr_callback_fun(void)
{
    JL_TIMER0->CON |= BIT(14);

    g_var.t0_cnt1++;

    delay_2ms_count();
    dac_fade(NULL);
    dac_delay_count();
    ir_timeout();
    adc_scan(NULL);
    /* rotate_dec_get_dat(); */

    if ((g_var.t0_cnt1 % 5) == 0) {
        sys_timer_schedule();
        poweron_charge_detect();
#if SYS_LVD_EN
        battery_check();
#endif
        dev_delay_10ms_count();
        dev_detect_fun(NULL);
        scan_capkey();
        key_scan(NULL);
        led_cnt(1);
        led_scan();
#if USB_PC_EN
        pc_check_api();
#endif
#if LINEIN_EN
        aux_detect();
#endif
    }

    if (g_var.t0_cnt1 == 250) {
        g_var.t0_cnt1 = 0;
        /* putchar('H'); */
        control_power_on_cnt(POWER_ON_CNT_INC, 0);
        power_off_demo_test();
        g_var.sys_halfsec = !g_var.sys_halfsec;//led7 driver
        /* put_event(EVENT_HALF_SECOND); */
        half_second_msg();
    }
}
void timer0_isr_fun(void)
{
    JL_TIMER0->CON |= BIT(14);
    if (timer_callback_fun != NULL) {
        timer_callback_fun();
    }

}
IRQ_REGISTER(IRQ_TIME0_IDX, timer0_isr_fun);

static void timer0_init(void)
{
    s32 timer_num;
    u32 prd_cnt, clk, tmp_tick;
    u8 index;
    u8 clk_src;
    u8 catch_flag = 0;

    //resrt_sfr
    JL_TIMER0->CON = 0;

    clk = TIMER_CLK;
    clk /= 1000;
    clk *= 2; //2ms
    for (index = 0; index < (sizeof(timer_div) / sizeof(timer_div[0])); index++) {
        prd_cnt = clk / timer_div[index];
        if (prd_cnt > MIN_TIME_CNT && prd_cnt < MAX_TIME_CNT) {
            catch_flag = 1;
            break;
        }
    }

#if SOURCE_CLK
    clk_src = TIMER_CLK_SRC_OSC;
#else
    clk_src = TIMER_CLK_SRC_SYSCLK;
#endif

    if (catch_flag == 0) {
        puts("warning:timer_err\n");
        return;
    }

    IRQ_REQUEST(IRQ_TIME0_IDX, timer0_isr_fun);

    JL_TIMER0->CNT = 0;
    JL_TIMER0->PRD = prd_cnt - 1;
    JL_TIMER0->CON = BIT(0) | (clk_src << 2) | (index << 4);

    /* otp_printf("JL_TIMER0->CNT = 0x%x\n", JL_TIMER0->CNT); */
    /* otp_printf("JL_TIMER0->PRD = 0x%x\n", JL_TIMER0->PRD); */
    /* otp_printf("JL_TIMER0->CON = 0x%x\n", JL_TIMER0->CON); */
}

void timer_delay_2ms(volatile u32 delay_time)
{
    while (delay_time) {
        put_u32hex(delay_time);
        if (JL_TIMER0->CON & BIT(15)) {
            JL_TIMER0->CON |= BIT(14);
            delay_time--;
            puts(".");
        }
    }
}

void timer_init(void (*isr_fun)(void))
{
    sys_timer_var_init() ;
    timer_callback_fun = isr_fun;
    timer0_init();
}


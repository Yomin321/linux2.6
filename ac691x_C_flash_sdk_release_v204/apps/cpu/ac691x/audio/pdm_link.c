#include "audio/pdm_link.h"
#include "irq_api.h"
#include "uart.h"
#include "audio/src.h"
#include "audio/dac_api.h"
#include <string.h>
#include "sdk_cfg.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".dac_app_bss")
#pragma data_seg(	".dac_app_data")
#pragma const_seg(	".dac_app_const")
#pragma code_seg(	".dac_app_code")
#endif


//输出到dac测试
#define OUTPUT_TO_DAC_TEST         0            //output data to dac

#if OUTPUT_TO_DAC_TEST
#define OUTPUT_TWO_CHL_ENABLE      1            //0:output one channel    1:output two channels
#endif

#define PLINK_OUTPUT_SR            SR8000       //dac sr

#define PLINK_DIV   	  16
#define PLINK_CHL   	  2
#define PLINK_LEN         64

s16 plink_buf[PLINK_CHL][2 * PLINK_LEN] AT(.dac_buf_sec) __attribute__((aligned(4)));


static void (*plink_data_callback)(s16 *, u16) = NULL;

void set_plink_data_callback(void (*callback)(s16 *, u16))
{
    plink_data_callback = callback;
}

static void src_output_cb(u8 *buf, u16 len, u8 flag)
{
    u8 cnt;
    if (flag & BIT(0)) {
        //output

#if OUTPUT_TO_DAC_TEST
#if OUTPUT_TWO_CHL_ENABLE
        dac_write((s16 *)buf, len);
#else
        u8 cnt;
        s16 l2d_buf[PLINK_LEN * 2];
        s16 *sp = (s16 *)buf;
        s16 *dp = (s16 *)l2d_buf;

        for (cnt = 0; cnt < (len / 2); cnt++) {
            dp[cnt * 2] = sp[cnt];
            dp[cnt * 2 + 1] = sp[cnt];
        }
        dac_write((s16 *)l2d_buf, len * 2);
#endif
#endif

        if (plink_data_callback) {
            plink_data_callback((s16 *)buf, len);
        }
    }

    //kick start
    if (flag == BIT(0)) {
        src_kick_start(1);
    } else {
        src_kick_start(0);
    }
}

static void plink_src_ctl(u8 flag, u16 in_rate, u16 out_rate)
{
    src_param_t src_p;

    if (flag) {
        puts("src_start\n");
#if OUTPUT_TWO_CHL_ENABLE
        src_p.in_chinc  = 1;
        src_p.in_spinc  = 2;
        src_p.out_chinc = 1;
        src_p.out_spinc = 2;

        src_p.nchannel  = 2;
#else
        src_p.in_chinc  = 1;
        src_p.in_spinc  = 1;
        src_p.out_chinc = 1;
        src_p.out_spinc = 1;
        src_p.nchannel  = 1;
#endif
        src_p.in_rate = in_rate;
        src_p.out_rate = out_rate;
        src_p.output_cbk = (void *)src_output_cb;
        src_enable(&src_p);
    } else {
        puts("src_off\n");
        src_disable();
    }
}

static void plink_isr_deal(s16 *buf0, u32 len0, s16 *buf1, u32 len1)
{
#if OUTPUT_TWO_CHL_ENABLE
    u8 cnt;

    s16 l2d_buf[PLINK_LEN * 2];
    s16 *sp0 = (s16 *)buf0;
    s16 *sp1 = (s16 *)buf1;
    s16 *dp  = (s16 *)l2d_buf;

    for (cnt = 0; cnt < PLINK_LEN; cnt++) {
        dp[cnt * 2] = sp0[cnt];
        dp[cnt * 2 + 1] = sp1[cnt];
    }

    /* if (is_dac_write_able(PLINK_LEN * 2 * 2) != 0)  */
    /* { */
    /* dac_write((s16 *)l2d_buf, PLINK_LEN * 2 * 2); */
    /* } */

    src_run((u8 *)l2d_buf, PLINK_LEN * 2 * 2);
#else
    src_run((u8 *)buf0, PLINK_LEN * 2);
#endif
}

static void plink_isr(void)
{
    s16 *res0;
    s16 *res1;
    u8 buf_flag;

    JL_PLNK->CON |= BIT(14);    //clr pending

    if (JL_PLNK->CON & BIT(9)) {
        buf_flag = 0;    //use buf0
    } else {
        buf_flag = 1;    //use buf1
    }

    //ch0
    res0 = (s16 *)plink_buf;
    res0 += buf_flag * PLINK_LEN;

    //ch1
    res1 = (s16 *)plink_buf + PLINK_LEN * 2;
    res1 += buf_flag * PLINK_LEN;

    plink_isr_deal(res0, PLINK_LEN, res1, PLINK_LEN);

}
IRQ_REGISTER(IRQ_PDM_LINK_IDX, plink_isr);

void pdm_link_enable(void)
{
    u32 sclk;
    JL_PLNK->LEN = PLINK_LEN;
    JL_PLNK->ADR = (volatile unsigned int)plink_buf;
    JL_PLNK->SMR = PLINK_DIV - 1;

    sclk = 48000000L / PLINK_DIV;

    JL_PORTA->DIR |= BIT(0) | BIT(2);    //DAT0 DAT1
    JL_PORTA->DIR &= ~BIT(1);            //SCLK

    JL_IOMAP->CON1 |= BIT(20);

    SFR(JL_PLNK->CON, 14, 1, 1); //CPND
    SFR(JL_PLNK->CON, 8, 1, 1);  //ie

    SFR(JL_PLNK->CON, 1, 1, 0);  //CH0 scale
    SFR(JL_PLNK->CON, 2, 2, 0);  //CH0 mode

    SFR(JL_PLNK->CON, 5, 1, 0);  //CH1 scale
    SFR(JL_PLNK->CON, 6, 2, 0);  //CH1 mode

    SFR(JL_PLNK->CON, 0, 1, 1);  //CH0EN
    SFR(JL_PLNK->CON, 4, 1, 1);  //CH1EN

    otp_printf("\nsclk:%d sr:%d\n", sclk, sclk / 50);
    plink_src_ctl(1, sclk / 50, PLINK_OUTPUT_SR);

#if OUTPUT_TO_DAC_TEST
    dac_set_samplerate(PLINK_OUTPUT_SR, 1);
#endif

    IRQ_REQUEST(IRQ_PDM_LINK_IDX, plink_isr);
}

void pdm_link_disable(void)
{
    JL_PLNK->CON = 0;
    plink_data_callback = NULL;
    plink_src_ctl(0, 0, 0);
}

void pdm_link_demo(void)
{
#if 0
    pdm_link_enable();
    extern void close_wdt(void);
    close_wdt();
    while (1);
#endif
}

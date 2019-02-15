#include "sdk_cfg.h"
#include "clock_api.h"
#include "rtc/rtc_api.h"
#include "adc_api.h"
#include "sdk_cfg.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif

/*config adc clk according to sys_clk*/
const u32 sys2adc_clk_info[] = {
    128000000L,
    96000000L,
    72000000L,
    48000000L,
    24000000L,
    12000000L,
    6000000L,
    1000000L,
};

u16 adc_value[5];
const u32 ad_table[] = {
#if KEY_AD_RTCVDD_EN
    AD_CH_PR2,
    AD_CH_RTCVDD,
#elif KEY_AD_VDDIO_EN
    AD_CH_PA9,
#endif
    AD_CH_VBAT,
    AD_CH_CHARGE,
    /* AD_CH_LDOIN, */
    AD_CH_LDOREF,
};

void adc_ch_sel(u32 ch)
{
    JL_ADC->CON &= ~0x0f00;            //ADC clear
    JL_ADC->CON |= ch;     		   //AD channel select
    JL_ADC->CON |= BIT(6);             //AD start
}

void adc_ldo_ch_sel(u32 ch)
{
    SFR(JL_SYSTEM->LDO_CON0, 4, 3, 0);
    SFR(JL_SYSTEM->LDO_CON0, 30, 1, 1);
    SFR(JL_SYSTEM->LDO_CON0, 1, 1, 1);
    if (ch == 0) {
        SFR(JL_SYSTEM->LDO_CON0, 0, 1, 1);
    } else {
        /* SFR(JL_SYSTEM->LDO_CON0, 0, 1, 0); */
    }
    SFR(JL_SYSTEM->LDO_CON0, 4, 3, ch);

}

u32 get_adc_clk(u32 lsb_fre)
{
    u32 adc_clk;
    u32 adc_clk_idx;
    u8 cnt;

    //ADC clk config
    adc_clk = lsb_fre;
    cnt = sizeof(sys2adc_clk_info) / sizeof(sys2adc_clk_info[0]);
    for (adc_clk_idx = 0; adc_clk_idx < cnt; adc_clk_idx++) {
        if (adc_clk > sys2adc_clk_info[adc_clk_idx]) {
            break;
        }
    }

    if (adc_clk_idx < cnt) {
        adc_clk_idx = cnt - adc_clk_idx;
    } else {
        adc_clk_idx = cnt - 1;
    }

    otp_printf("ADC_DIV = %d,clk:%d,lsb:%d\n", adc_clk_idx, sys2adc_clk_info[adc_clk_idx], lsb_fre);
    return adc_clk_idx;
}

void adc_init_api(u32 channel, u32 lsb_clk, u8 lvd_en)
{
    if (lvd_en) {
        //puts("adc_lvd_enable\n");
        adc_ldo_ch_sel(0);
    }

    JL_ADC->CON = 0;

    //init adc clk
    JL_ADC->CON &= ~(BIT(2) | BIT(1) | BIT(0));
    JL_ADC->CON |= (get_adc_clk(lsb_clk) & 0x7L);

    JL_ADC->CON |= (0XF << 12);  //Launch Delay Control
    JL_ADC->CON |= channel;      //ADC Channel Select
    JL_ADC->CON |= BIT(3);       //ADC Analog Module Enable
    JL_ADC->CON |= BIT(4);       //ADC Controller Enable
    JL_ADC->CON |= BIT(6);       //Launch ADC
}

void adc_init()
{
    adc_init_api(ad_table[0], LSB_CLK, SYS_LVD_EN);
}
void adc_off_api(void)
{
    JL_ADC->CON = 0;
}

u16 adc_res_api(u32 channel)
{
    u16 adc_value = 0;
    u16 adc_con_tmp = 0;

    while (!(BIT(7) & JL_ADC->CON));	  //wait pending
    adc_value = JL_ADC->RES;

    adc_con_tmp = JL_ADC->CON & (~0x0F00);
    JL_ADC->CON = adc_con_tmp | (channel & 0xFFFF);//channel; //AD channel select
    if ((channel & 0xFFFF) == AD_CH_RTC) {
        adc_mux_ch_set((channel & 0xFFFF0000) >> 16);
    } else if ((channel & 0xFFFF) == AD_CH_PMU) {
        adc_ldo_ch_sel((channel & 0xFFFF0000) >> 16);
    }
    JL_ADC->CON |= BIT(6);             //AD start

    return adc_value;
}

u8 next_channel;
volatile u8 adc_busy_flag = 0;
void adc_scan(void *param)
{
    if (adc_busy_flag) {
        return;
    }

    static u8 channel = 0;

    next_channel = channel + 1;

    if (next_channel == R_MAX_AD_CHANNEL) {
        next_channel = 0;
    }
    adc_value[channel++] = adc_res_api(ad_table[next_channel]);
    /* otp_printf("c=%d   v=%d\n",channel-1,adc_value[channel-1]); */

    if (channel == R_MAX_AD_CHANNEL) {
        channel = 0;
    }
}



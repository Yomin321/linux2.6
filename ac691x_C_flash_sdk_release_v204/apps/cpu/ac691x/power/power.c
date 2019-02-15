#include "power.h"
#include "rtc/rtc_api.h"
#include "charge.h"
/* #include "sys_detect.h" */
#include "audio/dac.h"
#include "power_manage_api.h"
#include "clock.h"
#include "charge.h"
#include "adc_api.h"
/* #include "dac.h" */
#include "audio/dac_api.h"
#include "msg.h"
#include "key_drv/key.h"
#include "led.h"
#include "bluetooth/avctp_user.h"
#include "crc_api.h"
#include "sys_cfg.h"
#include "dev_mg_api.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".power_driver_bss")
#pragma data_seg(	".power_driver_data")
#pragma const_seg(	".power_driver_const")
#pragma code_seg(	".power_driver_code")
#endif
#define DEBUG_RUN_FLASH_POARTB  1 //run flash 大封装片80P的  flash: PB0 PB1 PB2 PB6
#define DEBUG_RUN_FLASH_POARTD  2 //run flash 小封装片24P、20P的  flash: PD0 PD1 PD2 PD3

#define DEBUG_RUN_FLASH_POART  DEBUG_RUN_FLASH_POARTD //选择 对应使用的FLASH POART口


void in_low_pwr_port_deal(u8 mode)
{
    u32 porta_value = 0xffff;
    u32 portb_value = 0xffff;
    u32 portc_value = 0xffff;
    u32 portd_value = 0xffff;

    if (get_work_mode() == RUN_FLASH_MODE) { //跑flash ，进入powerdown不能关闭对应的flash IO口
        /* #if (DEBUG_RUN_FLASH_POART==DEBUG_RUN_FLASH_POARTD) */
        portd_value &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3));
        /* #elif(DEBUG_RUN_FLASH_POART==DEBUG_RUN_FLASH_POARTB) */
        portb_value &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(6));
        /* #endif */
    }

#if LED_EN

#if   (LEDB_PORT&LED_POARA)
    porta_value &= ~LED_B;

#elif (LEDB_PORT&LED_POARB)
    portb_value &= ~LED_B;

#elif (LEDB_PORT&LED_POARC)
    portc_value &= ~LED_B;

#elif (LEDB_PORT&LED_POARD)
    portd_value &= ~LED_B;
#endif


#if   (LEDR_PORT&LED_POARA)
    porta_value &= ~LED_R;

#elif (LEDR_PORT&LED_POARB)
    portb_value &= ~LED_R;

#elif (LEDR_PORT&LED_POARC)
    portc_value &= ~LED_R;

#elif (LEDR_PORT&LED_POARD)
    portd_value &= ~LED_R;
#endif
#endif
    /*
    put_u32hex(porta_value);
    put_u32hex(portb_value);
    put_u32hex(portc_value);
    put_u32hex(portd_value);
    */

    JL_PORTA->DIR |=  porta_value;
    JL_PORTA->PU &= ~porta_value;
    JL_PORTA->PD &= ~porta_value;
    JL_PORTA->DIE &= ~porta_value;


    JL_PORTB->DIR |=  portb_value;
    JL_PORTB->PU &= ~portb_value;
    JL_PORTB->PD &= ~portb_value;
    JL_PORTB->DIE &= ~portb_value;


    JL_PORTC->DIR |=  portc_value;
    JL_PORTC->PU &= ~portc_value;
    JL_PORTC->PD &= ~portc_value;
    JL_PORTC->DIE &= ~portc_value;

    /*注意spi flash的几个脚不能关*/
    JL_PORTD->DIR |= portd_value;
    JL_PORTD->PU  &= ~portd_value;
    JL_PORTD->PD  &= ~portd_value;
    JL_PORTD->DIE &= ~portd_value;

    /////////////////usb///////////////////
    JL_USB->IO_CON0 |= BIT(2) | BIT(3); //DP设置为输入//DP设置为输入
    JL_USB->IO_CON0 &= ~(BIT(7) | BIT(6) | BIT(5) | BIT(4));//close usb io PU PD

    JL_USB->CON0 = 0;
}

static void out_low_pwr_deal(u32 time_ms)
{
    /* key_init(); */
#if KEY_AD_RTCVDD_EN
    extern void ad_rtcvdd_key_filter(u8 cnt);
    ad_rtcvdd_key_filter(10);
#endif
}

static void low_pwr_deal(u8 mode, u32 timer_ms)
{
    if (mode) {
        /* putchar('i'); */
        in_low_pwr_port_deal(mode);
    } else {
        /* putchar('o'); */
        //otp_printf("timer_ms=%d\n", timer_ms);
        out_low_pwr_deal(timer_ms);
        get_jiffies(1, timer_ms);
    }
}

enum {
    WAKEUP_0 = 0,
    WAKEUP_1,
    WAKEUP_2,
    WAKEUP_3,
    WAKEUP_4,
    WAKEUP_5,
    WAKEUP_6,
    WAKEUP_7,
    WAKEUP_8,
    WAKEUP_9,
    WAKEUP_10,
    WAKEUP_11,
    WAKEUP_12,
    WAKEUP_13,
    WAKEUP_14,
    WAKEUP_15,
};

#define WAKEUP_UP_EDGE     0x0f
#define WAKEUP_DOWN_EDGE   0xf0
static void set_poweroff_wakeup_io()
{
    /* otp_printf("wk io\n"); */

    u8 wakeup_io_en = 0;
    u8 wakeup_edge = 0;

    //BIT(0)  PR0 : 0 disable  1 enable
    //BIT(1)  PR1 : 0 disable  1 enable
    //BIT(2)  PR2 : 0 disable  1 enable
    //BIT(3)  PR3 : 0 disable  1 enable
    /* wakeup_io_en |= WAKE_UP_PR0 | WAKE_UP_PR1 | WAKE_UP_PR2 | WAKE_UP_PR3; */
    wakeup_io_en |=  WAKE_UP_PR2;

    //BIT(4)  PR0 : 0 rising dege  1 failling edge
    //BIT(5)  PR1 : 0 rising dege  1 failling edge
    //BIT(6)  PR2 : 0 rising dege  1 failling edge
    //BIT(7)  PR3 : 0 rising dege  1 failling edge
    /* wakeup_edge |= EDGE_PR0 | EDGE_PR1 | EDGE_PR2 | EDGE_PR3;     //failling edge */
    /* wakeup_edge &= ~(EDGE_PR0 | EDGE_PR1 | EDGE_PR2 | EDGE_PR3);  //rising dege */
    wakeup_edge |= EDGE_PR2;     //failling edge

    soft_poweroff_wakeup_io(wakeup_io_en, wakeup_edge);

    charge_mode_detect_ctl(1);
}

/*enter sleep mode wakeup IO setting*/
static void enter_sleep_mode_set(u16 wakeup_cfg, u8 wakeup_edge)
{
#if 0
    close_wdt();

    dac_toggle(0); //close dac mudule


    JL_AUDIO->LADC_CON = 0;
    JL_AUDIO->ADA_CON0 = 0;
    /* JL_AUDIO->ADA_CON1 = 0; */
    /* JL_AUDIO->ADA_CON2 = 0; */

    JL_AUDIO->DAA_CON0 = 0;
    JL_AUDIO->DAA_CON1 = 0;
    JL_AUDIO->DAA_CON2 = 0;
    JL_AUDIO->DAA_CON3 = 0;
    /* JL_AUDIO->DAA_CON4 = 0; */
    /* JL_AUDIO->DAA_CON5 = 0; */

    JL_WAKEUP->CON0 = 0;      //wakeup enbale
    JL_WAKEUP->CON1 = 0;      //wakeup edge
    JL_WAKEUP->CON2 = 0xffff; //wakeup pending(clear)

    switch (wakeup_cfg) {
    case WAKEUP_0:
        break;

    case WAKEUP_1:
        break;

    case WAKEUP_2:
        otp_printf("WAKEUP_2\n");
        JL_WAKEUP->CON2 |= BIT(2);
        JL_PORTA->DIR |= BIT(8);
        JL_PORTA->DIE |= BIT(8);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTA->PU |= BIT(8);
            JL_PORTA->PD &= ~BIT(8);
            JL_WAKEUP->CON1 |= BIT(2);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTA->PU &= ~BIT(8);
            JL_PORTA->PD |= BIT(8);
            JL_WAKEUP->CON1 &= ~BIT(2);
        }
        JL_WAKEUP->CON0 |= BIT(2);
        break;

    case WAKEUP_3:
        otp_printf("WAKEUP_3\n");
        JL_WAKEUP->CON2 |= BIT(3);
        JL_PORTA->DIR |= BIT(10);
        JL_PORTA->DIE |= BIT(10);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTA->PU |= BIT(10);
            JL_PORTA->PD &= ~BIT(10);
            JL_WAKEUP->CON1 |= BIT(3);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTA->PU &= ~BIT(10);
            JL_PORTA->PD |= BIT(10);
            JL_WAKEUP->CON1 &= ~BIT(3);
        }
        JL_WAKEUP->CON0 |= BIT(3);
        break;

    case WAKEUP_4:
        otp_printf("WAKEUP_4\n");
        JL_WAKEUP->CON2 |= BIT(4);
        JL_PORTA->DIR |= BIT(14);
        JL_PORTA->DIE |= BIT(14);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTA->PU |= BIT(14);
            JL_PORTA->PD &= ~BIT(14);
            JL_WAKEUP->CON1 |= BIT(4);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTA->PU &= ~BIT(14);
            JL_PORTA->PD |= BIT(14);
            JL_WAKEUP->CON1 &= ~BIT(4);
        }
        JL_WAKEUP->CON0 |= BIT(4);
        break;
    case WAKEUP_5:
        break;
    case WAKEUP_6:
        break;
    case WAKEUP_7:
        break;
    case WAKEUP_8:
        otp_printf("WAKEUP_8\n");
        JL_WAKEUP->CON2 |= BIT(8);
        JL_PORTA->DIR |= BIT(3);
        JL_PORTA->DIE |= BIT(3);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTA->PU |= BIT(3);
            JL_PORTA->PD &= ~BIT(3);
            JL_WAKEUP->CON1 |= BIT(8);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTA->PU &= ~BIT(3);
            JL_PORTA->PD |= BIT(3);
            JL_WAKEUP->CON1 &= ~BIT(8);
        }
        JL_WAKEUP->CON0 |= BIT(8);
        break;
    case WAKEUP_9:
        otp_printf("WAKEUP_9\n");
        JL_WAKEUP->CON2 |= BIT(9);
        JL_PORTA->DIR |= BIT(5);
        JL_PORTA->DIE |= BIT(5);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTA->PU |= BIT(5);
            JL_PORTA->PD &= ~BIT(5);
            JL_WAKEUP->CON1 |= BIT(9);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTA->PU &= ~BIT(5);
            JL_PORTA->PD |= BIT(5);
            JL_WAKEUP->CON1 &= ~BIT(9);
        }
        JL_WAKEUP->CON0 |= BIT(9);
        break;
    case WAKEUP_10:
        otp_printf("WAKEUP_10\n");
        JL_WAKEUP->CON2 |= BIT(10);
        JL_PORTB->DIR |= BIT(0);
        JL_PORTB->DIE |= BIT(0);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTB->PU |= BIT(0);
            JL_PORTB->PD &= ~BIT(0);
            JL_WAKEUP->CON1 |= BIT(10);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTB->PU &= ~BIT(0);
            JL_PORTB->PD |= BIT(0);
            JL_WAKEUP->CON1 &= ~BIT(10);
        }
        JL_WAKEUP->CON0 |= BIT(10);
        break;
    case WAKEUP_11:
        otp_printf("WAKEUP_11\n");
        JL_WAKEUP->CON2 |= BIT(11);
        JL_PORTB->DIR |= BIT(2);
        JL_PORTB->DIE |= BIT(2);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTB->PU |= BIT(2);
            JL_PORTB->PD &= ~BIT(2);
            JL_WAKEUP->CON1 |= BIT(11);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTB->PU &= ~BIT(2);
            JL_PORTB->PD |= BIT(2);
            JL_WAKEUP->CON1 &= ~BIT(11);
        }
        JL_WAKEUP->CON0 |= BIT(11);
        break;
    case WAKEUP_12:
        otp_printf("WAKEUP_12\n");
        JL_WAKEUP->CON2 |= BIT(12);
        JL_PORTB->DIR |= BIT(9);
        JL_PORTB->DIE |= BIT(9);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTB->PU |= BIT(9);
            JL_PORTB->PD &= ~BIT(9);
            JL_WAKEUP->CON1 |= BIT(12);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTB->PU &= ~BIT(9);
            JL_PORTB->PD |= BIT(9);
            JL_WAKEUP->CON1 &= ~BIT(12);
        }
        JL_WAKEUP->CON0 |= BIT(12);
        break;
    case WAKEUP_13:
        otp_printf("WAKEUP_13\n");
        JL_WAKEUP->CON2 |= BIT(13);
        JL_PORTB->DIR |= BIT(11);
        JL_PORTB->DIE |= BIT(11);
        if (wakeup_edge == WAKEUP_DOWN_EDGE) {
            otp_printf("WAKEUP_DOWN_EDGE\n");
            JL_PORTB->PU |= BIT(11);
            JL_PORTB->PD &= ~BIT(11);
            JL_WAKEUP->CON1 |= BIT(13);
        } else {
            otp_printf("WAKEUP_UP_EDGE\n");
            JL_PORTB->PU &= ~BIT(11);
            JL_PORTB->PD |= BIT(11);
            JL_WAKEUP->CON1 &= ~BIT(13);
        }
        JL_WAKEUP->CON0 |= BIT(13);
        break;
    case WAKEUP_14:
        break;
    case WAKEUP_15:
        break;

    default:
        return;
    }
#endif
}

/*sleep mode wakeup io setting fuction*/
static void set_sleep_mode_wakeup_io()
{
    /* enter_sleep_mode_set(WAKEUP_3, WAKEUP_DOWN_EDGE); */
}

/*check wheather can enter lowpower*/
/*return 0:can not enter lowpower  1:can enter lowpower*/
extern u8 is_enter_fast_test(void);
static u32 wheather_can_enter_lowpower(void)
{
    if (1 == get_dev_mg_stats()) {		//dev is busy
        return 0;
    }

    if (get_key_invalid_flag()) {
        putchar('k');
        return 0;
    }
    if (get_ldo5v_online_flag()) {                   //充电线插入
        putchar('C');
        return 0;
    }
    if (is_enter_fast_test()) {			//fast test
        return 0;
    }
    /* if (!is_auto_mute()) { */
    /* putchar('a'); */
    /* return 0; */
    /* } */

    if (dac_ctl.toggle) {
        putchar('d');
        task_post_msg(NULL, 1, MSG_DAC_OFF);
        return 0;
    }

    putchar('D');
    return 1;
}

/*bt noconn enter powerdown callback*/
u32 bt_noconn_pwr_down_in(void)
{
    puts("pwr_d_in\n");

    if (is_enter_fast_test()) {			//fast test
        return 0;
    }

    //close dac module
    task_post_msg(NULL, 1, MSG_DAC_OFF);
    return 0;
}

/*bt noconn exit powerdown callback*/
u32 bt_noconn_pwr_down_out(void)
{
    puts("pwr_d_out\n");
    //open dac module
    task_post_msg(NULL, 1, MSG_DAC_ON);
    return 0;
}

extern u8 lmp_get_sniff_flag(void);
void get_powerdown_osc_info(u32 *osc, u32 *oshz)
{
    u8 flag = lmp_get_sniff_flag();

    if (flag) {
        *osc = LRC_32K;
        *oshz = 32000L;
    } else {
        *osc = BT_OSC;
        *oshz = 24000000L;
    }
}
/*
***********************************************************************************
*					PWR SETUP INIT
*
*Description: This function is called to init pwr_cfg when enter power_down
*
*Argument(s): none

*Returns	: none
*
*Note(s)	: 1)
***********************************************************************************
*/
struct _PWR_SETUP {
    u16 crc;
    /* 0(3.4v)  1(3.2v) 2(3.0v) 3(2.8v) 4(2.6v) 5(2.4v) 6(2.2v) 7(2.0v) */
    u8	vddio: 4;
    u8	rtcvdd: 4;
    /* 0(0.728v)  1(0.696v) 2(0.648v) 3(0.605v) 4(0.571v) 5(0.537v) 6(0.501v) 7(0.467v) */
    u8	wdvdd: 4;
    /* 0:152nA 1:308nA 2:660nA 3:660nA */
    u8	wvbg_cur: 4;
} _GNU_PACKED_;
typedef struct _PWR_SETUP PWR_SETUP;
PWR_SETUP *pwr_setup;
void pwr_setup_init(void)
{
    u8 idx = 0;
    u16 crc = 0;
    pwr_setup = (PWR_SETUP *)(cfg_info_addr[CFG_ADR_PWR_INFO]);
    //printf("PWR_BASE:0x%x\n",&pwr_info_base);
    printf("PWR_SETUP:0x%x\n", pwr_setup);
    put_buf((u8 *)pwr_setup, sizeof(PWR_SETUP) * 4);
    while (idx < 4) {
        crc = crc16((void *)((u8 *)&pwr_setup[idx].crc + 2), 2);
        if (pwr_setup[idx].crc == crc) {
            otp_printf("idx:%d\n", idx);
            otp_printf("vddio:%d\n", pwr_setup[idx].vddio);
            otp_printf("rtcvdd:%d\n", pwr_setup[idx].rtcvdd);
            otp_printf("wdvdd:%d\n", pwr_setup[idx].wdvdd);
            otp_printf("wvbg_cur:%d\n", pwr_setup[idx].wvbg_cur);
            set_lowpower_pd_ldo_level(pwr_setup[idx].vddio, pwr_setup[idx].rtcvdd, pwr_setup[idx].wdvdd, pwr_setup[idx].wvbg_cur);
            break;
        }
        idx++;
    }
}
/*power init fuction*/
QLZ(.qlz_init)
void power_init_app(u8 mode, u8 chargeV)
{
    //charge sw
    charge_mode_detect_ctl(1);

    //set enter lowpower mode  0:not enter lowpower 1:powerdown 2:poweroff
    set_lowpower_mode_config(BT_LOW_POWER_MODE);
    //set wheather can enter lowpower callback
    set_wheather_can_enter_lowpower_callback(wheather_can_enter_lowpower);

    //set lowepower osc info
    set_lowpower_osc(LOWPOWER_OSC_TYPE, LOWPOWER_OSC_HZ);
    //set lowepower delay arg
    set_lowpower_delay_arg(SYS_Hz / 1000000L);
    //set wheather keep 32k osc when enter softpoweroff
    set_lowpower_keep_32K_osc_flag(0);
    //设置进入powerdown的时候vddio和rtcvdd降到多少伏
    //0(3.4v)  1(3.2v) 2(3.0v) 3(2.8v) 4(2.6v) 5(2.4v) 6(2.2v) 7(2.0v)
    //wdvdd lev
    //0(0.728v)  1(0.696v) 2(0.648v) 3(0.605v) 4(0.571v) 5(0.537v) 6(0.501v) 7(0.467v)
    //wvbg cur
    //0 - 3 0:min ... 3:max
    set_lowpower_pd_ldo_level(0x08, 0x08, 0x02, 0x01);
    pwr_setup_init();
    //set wheather disable btosc when enter lowpower
    set_lowpower_btosc_dis(0);
    //set io status when enter lowpower
    set_lowpower_io_status_set_callback(low_pwr_deal);
    //set lowpower wakeup io callback
    set_lowpower_wakeup_io_callback(set_poweroff_wakeup_io, NULL/*set_sleep_mode_wakeup_io*/);

#if (LOWPOWER_OSC_TYPE == LRC_32K)
    /* set_get_osc_callback(NULL); */
    set_get_osc_callback(get_powerdown_osc_info);
#else
    set_get_osc_callback(NULL);
#endif


    //init power
    set_sys_pwrmd(mode);

#if ((SDMMC0_EN == 1)||(SDMMC1_EN == 1))
    set_main_ldo_en(1);
#endif

    /**
     * set sys ldo
     * vddio:0(3.4v),1(3.2v),2(3.0v),3(2.8v),4(2.6v),5(2.4v),6(2.2v),7(2.0v)
     * rtcvdd: 3.52v-3.33v-3.15v-2.96v-2.86v-2.72v-2.57v-2.47v */
    set_sys_ldo_level(SYS_VDDIO_LEVEL, SYS_RTCVDD_LEVEL);
#if CHARGE_PROTECT_EN
    if (chargeV != 0xff) {
        set_charge_V(chargeV);
    } else {
        if (get_work_mode() == RUN_OTP_MODE) { //跑OTP，trim不到充电电压进入死循环
            otp_printf("charge trim err");
            while (1);
        }
    }
#endif

}


/*----------------------------------------------------------------------------*/
/**@brief   获取电池电量
   @param   void
   @param   void
   @return  电池电量值
   @note    tu8 get_battery_level(void)
*/
/*----------------------------------------------------------------------------*/
u16 get_battery_level(void)
{
    u16 battery_value, LDOIN_12, LDO_ref;

    LDOIN_12 = adc_value[R_AD_CH_VBAT];
    LDO_ref  = adc_value[R_AD_CH_LDOREF];//0x181,1.2v
    ////注意：vddio和rtc_vdd的配置影响电压检测
    if ((get_vddio_level() > 5) || (get_rtcvdd_level() > 5)) {
        battery_value = (((u16)LDOIN_12 * 460)) / ((u16)LDO_ref);    //针对AC69
    } else {
        battery_value = (((u16)LDOIN_12 * 488)) / ((u16)LDO_ref);    //针对AC69
    }
    /* otp_printf("battery_value:%d    %d   %d\n", battery_value, get_vddio_level(), get_rtcvdd_level()); */
    return battery_value;
}

#if SYS_LVD_EN

#define LOW_POWEROFF_VALUE   340  //低电关机电压
#define POWER_CHECK_CNT      100  //POWER_CHECK_CNT*10ms
u16 unit_cnt = 0;                 //计数单位时间POWER_CHECK_CNT*10ms
u16 low_warn_cnt = 0;             //单位时间内监测到报警电压次数
u16 low_off_cnt = 0;              //单位时间内监测到关机电压次数
u16 low_pwr_cnt = 0;
u16 normal_pwr_cnt = 0;
/* extern u8 is_pwrtimer_en(); */
static u8 low_voice_cnt = 0;
volatile u8 low_power_flag = 0;
u32 power_on_cnt = 500;
u16 low_power_off_value = LOW_POWEROFF_VALUE;
u16 low_warn_value  = (LOW_POWEROFF_VALUE + 10);


/*----------------------------------------------------------------------------*/
/**@brief  pwr config
   @param  default_level=1:reset to default pwr output
   		   default_level=0:reduce pwr output
   @return void
   @note
*/
/*----------------------------------------------------------------------------*/
/*3.53v-3.34v-3.18v-3.04v-2.87v-2.73v-2.62v-2.52v*/
//extern void set_sys_ldo_level(u8 level);

volatile u8 low_power_set = 0;
u8 get_pwr_config_flag()
{
    return low_power_set;
}
void pwr_level_config(u8 default_level)
{
#if 0
    //default config
    if (default_level && low_power_set) {
        puts(">>>>>>>>>Normal_power\n");
        low_power_set = 0;
        set_sys_ldo_level(SYS_LDO_NORMAL_LEVEL, SYS_LDO_NORMAL_LEVEL);	//level:0~7
    } else if (!low_power_set && !default_level) {
        puts(">>>>>>>>>Lower_power\n");
        low_power_set = 1;
        set_sys_ldo_level(SYS_LDO_REDUCE_LEVEL, SYS_LDO_REDUCE_LEVEL);	//level:0~7
    }
#endif
}

struct _LOW_PWR_SETUP {
    u16 crc;
    u8 low_power_off_value;			/*pwroff voltage*/
    u8 low_warn_value;			/*warn voltage	*/
} _GNU_PACKED_;
typedef struct _LOW_PWR_SETUP LOW_PWR_SETUP;
LOW_PWR_SETUP *low_pwr_setup;
/**
  *	Read low_pwr voltage value from CFG Zone
  */
void lowpwr_setup_init(void)
{
    u8 idx = 0;
    u16 crc = 0;
    low_pwr_setup = (LOW_PWR_SETUP *)cfg_info_addr[CFG_ADR_LOW_PWR];
    printf("low_pwr_base:0x%x\n", low_pwr_setup);
    low_power_off_value = LOW_POWEROFF_VALUE;
    low_warn_value  = (LOW_POWEROFF_VALUE + 10);
    put_buf((u8 *)low_pwr_setup, sizeof(PWR_SETUP) * 4);
    while (idx < 4) {
        crc = crc16((void *)((u8 *)&low_pwr_setup[idx].crc + 2), 2);
        if (low_pwr_setup[idx].crc == crc) {
            otp_printf("idx:%d\n", idx);
            otp_printf("low_power_off_value:%d\n", low_pwr_setup[idx].low_power_off_value);
            otp_printf("low_warn_value:%d\n", low_pwr_setup[idx].low_warn_value);
            if (low_pwr_setup[idx].low_warn_value >= 30 && low_pwr_setup[idx].low_warn_value <= 40) {
                low_warn_value = (low_pwr_setup[idx].low_warn_value * 10);
            }
            if (low_pwr_setup[idx].low_power_off_value >= 30 && low_pwr_setup[idx].low_power_off_value <= 40) {
                low_power_off_value = (low_pwr_setup[idx].low_power_off_value * 10);
            }
            break;
        }
        idx++;
    }
}

void battery_check(void)
{
    u16 val;
    u8 cnt;

    val = get_battery_level();
    /* charge_disconnect_bt_check(); */

    if (power_on_cnt > 0) {
        power_on_cnt--;
        return;
    }

    unit_cnt++;

    if (val < low_power_off_value) {
        low_off_cnt++;
    }
    if (val <= low_warn_value) {
        low_warn_cnt++;
    }

    if (unit_cnt >= POWER_CHECK_CNT) {
        if (is_sniff_mode()) {
            cnt = 4;
        }
        /* else if(()) */
        /* cnt = 4; */
        else {
            cnt = 10;
        }

        if (low_off_cnt > POWER_CHECK_CNT / 2) { //低电关机
            puts("\n*******Low Power********\n");
            low_power_flag = 1;
            bt_msg_power_off();
        } else if (low_warn_cnt > POWER_CHECK_CNT / 2) { //低电提醒
            low_power_flag = 1;

            low_voice_cnt ++;
            if (low_voice_cnt > cnt) {
                puts("\n**Low Power,Please Charge Soon!!!**\n");
                low_voice_cnt = 0;
                /* put_msg_fifo(MSG_LOW_POWER_VOICE); */
                task_post_msg(NULL, 1, MSG_LOW_POWER_VOICE);
            }
        } else {
            if (low_power_flag) {
                lower_power_led_flash(1, 0);
                /* R_LED_OFF(); */
                /* B_LED_OFF(); */
            }
            low_power_flag = 0;
        }

#if	0
        if (low_pwr_cnt > POWER_CHECK_CNT / 2) {
            pwr_level_config(0);
            normal_pwr_cnt = 0;
        } else {
            if (val > 350) {
                if (normal_pwr_cnt++ > 5) { //make sure battery full enough
                    normal_pwr_cnt = 0;
                    pwr_level_config(1);
                }
            } else {
                normal_pwr_cnt = 0;
            }
        }
#endif

        unit_cnt = 0;
        low_off_cnt = 0;
        low_warn_cnt = 0;
    }
}
#endif


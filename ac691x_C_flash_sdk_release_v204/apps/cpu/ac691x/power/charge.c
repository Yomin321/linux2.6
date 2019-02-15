#include "charge.h"
#include "power.h"
#include "sdk_cfg.h"
#include "rtc/rtc_api.h"
#include "led/led.h"
/* #include "sys_detect.h" */
#include "dac.h"
#include "key_drv/key.h"
#include "key_drv/key_drv_io.h"
#include "key_drv/key_drv_ad.h"
#include "audio/dac_api.h"
#include "clock.h"
#include "timer.h"
#include "adc_api.h"
#include "irq_api.h"
#include "clock_api.h"
#include "power_manage_api.h"
/* #include "usb/usb_global_var.h" */


//该宏配置是否开着机充电，当需要保持开机状态充电时，定义该宏
/* #define POWER_ON_CHARGE */


/* #define CHARGEE_DBG */
#ifdef  CHARGEE_DBG
#define charge_putchar        putchar
#define charge_printf         otp_printf
#define charge_buf            printf_buf
#else
#define charge_putchar(...)
#define charge_printf(...)
#define charge_buf(...)
#endif    //CHARGEE_DBG





typedef struct _charge_var {
    u32 cur_current;//当前的充电电流挡，有四档作为选择，0最小，3最大
    u32 full_cnt;//充满计数，在持续计数full_cnt个采集数据低于POWEROFF_THRESHOLD_VALU或者POWERON_THRESHOLD_VALUE后认为电池充满
    u8 power_mode;
    u16 charge_full;
} charge_var;

charge_var charge;

static void delay_ms()
{
    //Timer2 for delay
    JL_TIMER2->CON = BIT(14);
    JL_TIMER2->PRD = 375;
    JL_TIMER2->CNT = 0;
    SFR(JL_TIMER2->CON, 2, 2, 2); //use osc
    SFR(JL_TIMER2->CON, 4, 4, 3); //div64
    SFR(JL_TIMER2->CON, 14, 1, 1); //clr pending
    SFR(JL_TIMER2->CON, 0, 2, 1); //work mode
    while (!(JL_TIMER2->CON & BIT(15)));
    JL_TIMER2->CON = BIT(14);
}

void delay_nms(u32 n)
{
    while (n--) {
        delay_ms();
    }
}


extern u16 low_power_off_value ;
static void check_power_on_voltage(void)
{
    u16 val = 0;
    u8 tmp;

    u8 low_power_cnt = 0;
    u32 normal_power_cnt = 0;
    u32 delay_2ms_cnt = 0;

    LED_INIT_EN();

    while (!get_ldo5v_online_flag()) {
        clear_wdt();
        delay_nms(2); //2ms
        delay_2ms_cnt++;
        adc_scan(NULL);

        if ((delay_2ms_cnt % 5) == 0) { //10ms
            delay_2ms_cnt = 0;
            /*battery check*/
#if SYS_LVD_EN
            val = get_battery_level();
            if (val < low_power_off_value) {
                low_power_cnt++;
                if (low_power_cnt > C_POWER_BAT_CHECK_CNT) {
                    B_LED_OFF();
                    R_LED_ON();
                    delay_nms(1000); //1s
                    enter_sys_soft_poweroff();//soft_power_ctl(PWR_OFF);
                }
            } else
#endif
            {
                low_power_cnt++;
                normal_power_cnt++;
                if (normal_power_cnt > C_POWER_BAT_CHECK_CNT) { //normal power
                    delay_2ms_cnt = 0;
                    POWER_KEY_INIT();
                    while (1) {
                        clear_wdt();
                        delay_nms(2); //2ms

                        if (IS_POWER_KEY_DOWN()) {
                            putchar('+');
                            delay_2ms_cnt++;
                            if (delay_2ms_cnt > C_POWER_KEY_CHECK_CNT) {
                                charge.power_mode = POWER_ON;
                                R_LED_OFF();
                                B_LED_ON();
                                return;
                            }
                        } else {
                            putchar('-');
                            delay_2ms_cnt = 0;
                            enter_sys_soft_poweroff();//soft_power_ctl(PWR_OFF);
                        }
                    }
                }
            }
        }
    }
}


#if CHARGE_PROTECT_EN

static void charge_var_init(void)
{
    charge.cur_current = 0xff;
    charge.full_cnt = 0;
    charge.power_mode = POWER_OFF;
    charge.charge_full = get_cfg_charge_full();
}

u32 get_ldo5v_online_flag(void)
{
    return get_ldo5v_detect_flag();
}

void charge_mode_detect_ctl(u8 sw)
{
    rtc_ldo5v_detect(sw, 0);
}

static void charge_full_deal(void)
{
    charge_printf("--full--\n");
    R_LED_OFF();
    B_LED_ON();
}
static void charge_start(void)
{
    LED_INIT_EN();
    B_LED_OFF();
    R_LED_ON();
}

u8 power_on_check(void)
{
    return charge.power_mode;
}

u8 power_off_check(void)
{
    return charge.power_mode;
}

u16 max_charge_value = 0;
u16 min_charge_value = 0xffff;
u32 total_charge_value = 0;

void charge_full_det(u16 value)
{
    u16 charge_det_value = 0xffff;
    if (value > max_charge_value) {
        max_charge_value = value;
    }
    if (value < min_charge_value) {
        min_charge_value = value;
    }
    total_charge_value += value;

    charge.full_cnt++;

    if (charge.full_cnt > 1000 - 1) { //
        charge_det_value = (total_charge_value - max_charge_value - min_charge_value) / (charge.full_cnt - 2);

        if (((charge_det_value < charge.charge_full) && (power_off_check() == POWER_OFF))
            || ((charge_det_value < POWERON_THRESHOLD_VALUE) && (power_off_check() == POWER_ON))) {
            charge.full_cnt = 0;
            charge_full_deal();
        } else {
            charge.full_cnt = 0;
        }
        charge_det_value = 0xffff;
        max_charge_value = 0;
        min_charge_value = 0xffff;
        total_charge_value = 0;
    }
}
static void charge_deal()
{
    u16 vbat, vbat_threshold0, vbat_threshold1, vbat_threshold2, vbat_threshold3;

    vbat = get_battery_level();
    /* charge_printf("%x ",vbat); */

    if ((get_vddio_level() > 5) || (get_rtcvdd_level() > 5)) {
        vbat_threshold0 = 290;
        vbat_threshold1 = 0;
        vbat_threshold2 = 380;
        vbat_threshold3 = 450;
    } else {
        vbat_threshold0 = 320;
        vbat_threshold1 = 0;
        vbat_threshold2 = 400;
        vbat_threshold3 = 450;
    }

    if (vbat < vbat_threshold0) {
        if (charge.cur_current != 0) {
            charge_start();
            set_charge_A(1);
            set_charge_det(1);
            charge_printf("cur_current = 0\n ");
            charge.cur_current = 0;
        }
    } else if (vbat < vbat_threshold2) {
        if (charge.cur_current != 2) {
            charge_start();
            set_charge_A(2);
            set_charge_det(1);
            charge_printf("cur_current = 2\n ");
            charge.cur_current = 2;
        }
    } else {
        if (charge.cur_current != 3) {
            charge_start();
            set_charge_A(3);
            set_charge_det(1);
            charge.cur_current = 3;
            charge_printf("cur_current = 3\n ");
        }
        charge_full_det(adc_value[R_AD_CH_CHARGE]);

    }
}

void poweroff_charge_detect(void)
{
    static u32 power_off_cnt = 0;
    static u32 cnt_2ms = 0;
    static u32 cnt_1min = 0;
    static u32 det = 0;
    u8 val;
    cnt_2ms++;

    adc_scan(NULL);
    if ((cnt_2ms % 5) == 0) {
        if (get_ldo5v_online_flag()) {
            charge_deal();
            det += adc_value[R_AD_CH_CHARGE];
            if (cnt_2ms > 6000 * 5) {
                cnt_2ms = 0;
                charge_printf("time%d:vbat=%d det=%d\n", ++cnt_1min, get_battery_level(), det / 6000);
                det = 0;
            }
        }
        if (!get_ldo5v_online_flag()) {
            power_off_cnt ++;
            if (power_off_cnt > 20) {
                charge_printf("ldo5v_offline\n");
                charge_var_init();
                enter_sys_soft_poweroff();//soft_power_ctl(PWR_OFF);
            }
        } else {
            power_off_cnt = 0;
        }
    }
}

void poweron_charge_detect(void)
{
    static u32 power_off_cnt = 0;
    static u32 cnt_10ms = 0;
    static u32 cnt_1min = 0;
    static u32 det = 0;
    u8 val;

    if (get_ldo5v_online_flag()) {
        charge_deal();
        det += adc_value[R_AD_CH_CHARGE];
        cnt_10ms++;
        if (cnt_10ms > 6000) {
            cnt_10ms = 0;
            charge_printf("time%d:vbat=%d det=%d\n", ++cnt_1min, get_battery_level(), det / 6000);
            det = 0;
        }

#ifndef POWER_ON_CHARGE
        power_off_cnt ++;
        if (power_off_cnt > 20) {
            charge_printf("power_reset\n");
            JL_POWER->CON |= BIT(4);
        }
#endif//POWER_ON_CHARGE

    } else {
        power_off_cnt = 0;
        cnt_10ms = 0;
        cnt_1min = 0;
        det = 0;
    }


}

static void charge_power_idle()
{
    u32 key_cnt = 0;
    charge_var_init();

    timer_init(poweroff_charge_detect);

    charge_start();

    while (1) {
        clear_wdt();
        __asm__ volatile("idle");
        __asm__ volatile("nop");
        __asm__ volatile("nop");

#ifdef POWER_ON_CHARGE
        if (IS_POWER_KEY_DOWN()) {
            putchar('+');
            key_cnt ++;
            if (key_cnt > C_POWER_KEY_CHECK_CNT) {
                JL_POWER->CON |= BIT(4);
                return;
            }
        }
#endif
    }
}

extern void in_low_pwr_port_deal(u8 mode);
static void set_io_to_low_power_mode()
{
    SFR(WLA_CON17, 10, 4, 0x0);   //osc HCS
    SFR(WLA_CON17, 0, 5, 0x7);    //osc CLS
    SFR(WLA_CON17, 5, 5, 0x7);    //osc CRS
    SFR(WLA_CON14, 13, 1, 0x0);   //osc bt oe
    SFR(WLA_CON14, 14, 1, 0x1);   //osc fm oe
    SFR(WLA_CON17, 14, 2, 0x0);   //osc LDO level
    SFR(WLA_CON14, 11, 1, 0x0);   //osc ldo en
    SFR(WLA_CON14, 12, 1, 0x0);   //osc test
    SFR(WLA_CON18, 2, 2, 0x0);    //osc xhd current
///////注意这里必须关dac、、、、、
    /* dac_off_control(); //close dac mudule */
    dac_off_pre_api();
    dac_off_post_api();

    JL_AUDIO->LADC_CON = 0;
    JL_AUDIO->DAC_CON = 0;
    JL_AUDIO->ADA_CON0 = 0;
    JL_AUDIO->ADA_CON1 = 0;
    /* JL_AUDIO->ADA_CON2 = 0; */

    JL_AUDIO->DAA_CON0 = 0;
    JL_AUDIO->DAA_CON1 = 0;
    JL_AUDIO->DAA_CON2 = 0;
    JL_AUDIO->DAA_CON3 = 0;
    /* JL_AUDIO->DAA_CON4 = 0; */
    /* JL_AUDIO->DAA_CON5 = 0; */

    SFR(JL_FMA->CON1, 12, 1,  0x0);

    in_low_pwr_port_deal(0);

    PORTR_PU(PORTR0, 0);
    PORTR_PD(PORTR0, 0);
    PORTR_DIR(PORTR0, 1);
    PORTR_DIE(PORTR0, 0);

    PORTR_PU(PORTR1, 0);
    PORTR_PD(PORTR1, 0);
    PORTR_DIR(PORTR1, 1);
    PORTR_DIE(PORTR1, 0);

    PORTR_PU(PORTR3, 0);
    PORTR_PD(PORTR3, 0);
    PORTR_DIR(PORTR3, 1);
    PORTR_DIE(PORTR3, 0);
}


/* mode 1:power on     2:power off*/
void ldo5v_detect_deal()
{
    u8 ret = 0;
    u8 i = 0;
    u8 online_cnt = 0;
    u8 change_clk_flag = 0;
    u32 delay_2ms_cnt = 0;

    otp_printf("----01\n");

    charge_var_init();
    //check wheather ldo5v online
    for (i = 0; i < 10; i++) {
        delay_nms(2);
        if (get_ldo5v_online_flag()) {
            online_cnt++;
        }
    }
    charge_printf("online_cnt=%d\n", online_cnt);

    if (online_cnt > 3) {
#ifdef POWER_ON_CHARGE
        POWER_KEY_INIT();
        while (IS_POWER_KEY_DOWN()) {
            clear_wdt();
            delay_nms(2); //2ms
            putchar('+');
            delay_2ms_cnt++;
            if (delay_2ms_cnt > C_POWER_KEY_CHECK_CNT) {
                charge.power_mode = POWER_ON;
                R_LED_OFF();
                B_LED_ON();
                return;
            }
        }
#endif

        otp_printf("----04\n");
        set_sys_pwrmd(PWR_LDO15);
        set_io_to_low_power_mode();

        clock_init_app(SYS_CLOCK_INPUT_BT_OSC, OSC_Hz, 24000000L);

        set_sys_ldo_level(1, 1);
        set_vdd_level(7);
        set_dvdda_level(7);
        set_charge_det(1);
#ifdef  CHARGEE_DBG
        uart_model_int();
#endif
        charge_power_idle();
        change_clk_flag = 1;
    } else {
        otp_printf("----05\n");
        check_power_on_voltage();
    }
    return;
}

#else

u32 get_ldo5v_online_flag(void)
{
    return 0;
}

void charge_mode_detect_ctl(u8 sw)
{

}

u8 power_on_check(void)
{
    return 1;
}

u8 power_off_check(void)
{
    return 1;
}

void poweron_charge_detect(void)
{
}

void ldo5v_detect_deal()
{
    check_power_on_voltage();
}

#endif

static u8 pull_up_value = (MAX_MODE_CHANCE - 1);
u8 get_pull_up_num()
{
#if CHECK_PULL_UP_EN
    if (pull_up_value >= MAX_MODE_CHANCE) {
        pull_up_value = MAX_MODE_CHANCE - 1;
    }
#endif
    return pull_up_value;
}
bool check_pull_up_value(void)
{
#if CHECK_PULL_UP_EN
    puts("check_pull_up_value 1\n");
    u32 normal_power_cnt = 0;
    u32 delay_2ms_cnt = 0;
    static u8 check_pull_up_cnt = 0;
    while (IS_POWER_KEY_DOWN()) {
        putchar('w');
        clear_wdt();
    }
    set_sys_ldo_level(3, 3);
    check_ad_up_control(1);
    /* while (!get_ldo5v_online_flag()) { */
    while (1) {
        clear_wdt();
        delay_2ms(1);
        delay_2ms_cnt++;
        adc_scan(NULL);
        if (check_pull_up_cnt >= (R_MAX_AD_CHANNEL * 4)) {
            puts("check_pull_up_over\n");
            set_sys_ldo_level(SYS_VDDIO_LEVEL, SYS_RTCVDD_LEVEL);
            check_ad_up_control(0);
            return TRUE;
        } else if (check_pull_up_cnt >= R_MAX_AD_CHANNEL) {
            pull_up_value = get_pull_up_value();

        }
        check_pull_up_cnt++;
    }
    return TRUE;
#else
    return TRUE;
#endif

}


#include "board_init.h"
#include "audio/dac.h"
#include "audio/dac_api.h"
#include "common/common.h"
#include "timer.h"
#include "key_drv/key.h"
#include "led.h"
#include "rotate_dec.h"
#include "sdk_cfg.h"
#include "adc_api.h"
#include "power/charge.h"
#include "rtc/rtc_api.h"
#include "memory_api.h"
#include "sys_cfg.h"
#include "dev_manage.h"
#include "fs/fs.h"
#include "bluetooth/avctp_user.h"
#include "updata.h"
#include "eq.h"
#include "task_bt.h"


#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif
void pa_umute(void)
{
#if PA_MUTE_CFG
    PORTR_PU(PORTR1, 0);
    PORTR_PD(PORTR1, 0);
    PORTR_DIR(PORTR1, 0);
#if (PA_MUTE_CFG&PA_MUTE_HIGH)
    PORTR_OUT(PORTR1, 0);
#else
    PORTR_OUT(PORTR1, 1);
#endif
#endif
}

void pa_mute(void)
{
#if PA_MUTE_CFG
    PORTR_PU(PORTR1, 0);
    PORTR_PD(PORTR1, 0);
    PORTR_DIR(PORTR1, 0);
#if (PA_MUTE_CFG&PA_MUTE_HIGH)
    PORTR_OUT(PORTR1, 1);
#else
    PORTR_OUT(PORTR1, 0);
#endif
#endif
}

void usb_2_io(void)
{
    JL_USB->CON0 |= BIT(0);
    JL_USB->IO_CON0 |= BIT(11) ; //USB_IO_MODE
}
///根据不同封装设置一些双绑脚IO,和mic绑在一起，设为高阻
void set_port_init()
{

#ifndef UART_TXPA3_RXPA4
    JL_PORTA->DIR |=  BIT(3);///
    JL_PORTA->PD  &= ~BIT(3);
    JL_PORTA->PU  &= ~BIT(3);
#endif

    JL_PORTA->DIR |=  BIT(0);//MIC IO
    JL_PORTA->PD  &= ~BIT(0);
    JL_PORTA->PU  &= ~BIT(0);
#ifndef UART_USBP_USBM
    usb_2_io();//关闭usb的功能，作为普通IO口
    USB_DP_PU(0);
    USB_DP_PD(0);
    USB_DP_DIR(1);//6919 MIC 和DP绑在一起,DP设为高阻
#endif
    R_LED_OFF();
    B_LED_OFF();

}

static void flash_file_read(void)
{
    eq_cfg_read();
}

#include "audio/tone.h"
extern void in_low_pwr_port_deal(u8 mode);
QLZ(.qlz_init)
void board_init()
{
#if SYS_LVD_EN
    lowpwr_setup_init();
#endif
    adc_init();
    if (device_is_first_start() || get_updata_flag()) {
        otp_printf("\r\n**********device_is_first_start*************\n");
    } else {
        ldo5v_detect_deal();
    }

    /* set_lvd_mode(1, 4); */
    key_init();

    timer_init(timer0_isr_callback_fun);

    /* rotate_dec_init(RDEC_POL_PU); */
    /* dev_manage_init(); */
    dac_automute_set(AUTO_MUTE_CFG, 4, 1200, 200);

    audio_init();
    pa_umute();


    update_result_deal();

    /* led_fre_set(C_RLED_FAST_DOBLE_MODE); */
    led_fre_set(C_ALL_ON_MODE);

#ifdef SURPORT_DEVICE
    /* extern void fs_init_all(void); */
    /* extern void dev_manage_init(void); */
    fs_init_all();
    dev_manage_init();
#endif

    flash_file_read();
    eq_init();
    power_on_ignore_msg();
    resource_manage_init_app();
}

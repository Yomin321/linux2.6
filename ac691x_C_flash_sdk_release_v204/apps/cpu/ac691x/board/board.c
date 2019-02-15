#include "sdk_cfg.h"
#include "common/common.h"
#include "clock.h"
#include "hw_cpu.h"
#include "cpu.h"
#include "irq.h"
#include "irq_api.h"
#include "wdt.h"
#include "power.h"
#include "board_init.h"
#include "task_manager.h"
#include "nv_mem.h"
#include "task_bt.h"
#include "led.h"
#include "msg.h"
#include "uart_param.h"
#include "uart.h"
#include "audio/pdm_link.h"
#include "audio/dac_cpu.h"
#include "rtc_api.h"
#include "power_manage_api.h"
#include "charge.h"
#include "icache_interface.h"
#include "compress.h"
#include "memory_api.h"
#include "flash_api.h"
#include "sys_cfg.h"
#include "updata.h"


#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif

extern u8 READ_PMU_RESET_SOURCE(void);
extern u8 GET_LVD_CON(void);
extern void OTP_CONTROL(u8 ctl);


u32 args[3];
BOOT_ARG boot_arg_list sec_used(.reserved_info);
OTP_ARG otp_arg_list sec_used(.otp_info);

/*----------------------------------------------------------------------------*/
QLZ(.qlz_init)
void reset_source_check(void)
{
    if (JL_SYSTEM->RST_SRC == 0) {
        otp_printf("POWERUP RST\n");
        return;
    }

    if (JL_SYSTEM->RST_SRC & BIT(2)) {
        otp_printf("VCM RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(3)) {
        otp_printf("4S OR 8S RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(6)) {
        otp_printf("LVD RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(7)) {
        otp_printf("WTD RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(8)) {
        otp_printf("SOFT RST\n");
    } else if (JL_SYSTEM->RST_SRC & BIT(9)) {
        otp_printf("POWEROFF RST\n");
    } else {
        otp_printf("OTHER RST : %x\n", JL_SYSTEM->RST_SRC);
    }
}


static u8 app_mode = 0;

AT_POWER_DOWN
u8 get_work_mode(void)
{
    return app_mode;
}

void set_33v_clk_div(void)
{
    extern void P33_CLK_INIT(u8 div);
    extern void RTC_CLK_INIT(u8 div);
    extern void PMU_CLK_INIT(u8 div);

    u32 lsb_clk =  clock_get_lsb_freq();
    u8 div;

    div = lsb_clk / 24000000L;

    otp_printf("lsb_clk:%d div:%d\n", lsb_clk, div);

    P33_CLK_INIT(div);
    RTC_CLK_INIT(div);
    PMU_CLK_INIT(div);

}

void board_main(u32 cfg_addr, u32 addr, u32 res, u32 update_flag)
{
    PNS10k_EN(1);
#ifndef FLASH_MODE
    icache_way_available(1);
#endif
    set_port_init();
    args[0] = cfg_addr;
    args[1] = addr;
    args[2] = res;

    bank_switch(QLZ_INIT);

    clock_init_app(SYS_CLOCK_IN, OSC_Hz, SYS_Hz);
    uart_model_int();

#ifdef __DEBUG
    clock_dump();
#endif

    set_33v_clk_div();

    if ((u32)board_main > 0x1ff0000) {
        app_mode = RUN_OTP_MODE;
        otp_printf("\r\n****************************OTP_PowerUp***********************************\n");
    } else {
        app_mode = RUN_FLASH_MODE;
        otp_printf("\r\n***************************FLASH_PowerUp***********************************\n");
        OTP_CONTROL(0);
    }

    updata_result_get(update_flag);


    /* if (device_is_first_start()) { */
    /* otp_printf("\r\n**********device_is_first_start*************\n"); */
    /* } */

    flash_storage_init(cfg_addr);
    parse_sys_cfg(cfg_addr);
    cfg_info_init(addr, cfg_addr);
#if 0
    u8 lvd = GET_LVD_CON();
    lvd = (lvd & 0x38) >> 3;
    printf("maskrom lvd lev:%d\n", lvd);
#endif

    otp_printf("boot_arg_list->osc_type : %d\n", boot_arg_list.osc_type);
    otp_printf("boot_arg_list->dev_type : %d\n", boot_arg_list.dev_type);
    otp_printf("boot_arg_list->spi_post : %d\n", boot_arg_list.spi_port);
    otp_printf("boot_arg_list->osc_freq : %d\n", boot_arg_list.osc_freq);
    otp_printf("boot_arg_list->file_size: %d\n", boot_arg_list.file_size);
    otp_printf("boot_arg_list->boot_method: %d\n", boot_arg_list.boot_method);
    otp_printf("otp_arg_list-> : %x\n", otp_arg_list.lvd);
    otp_printf("otp_arg_list-> : %x\n", otp_arg_list.chargeA);
    otp_printf("otp_arg_list-> : %x\n", otp_arg_list.chargeV);

#ifdef __DEBUG
    reset_source_check();
    otp_printf("pmu reset : %x\n", READ_PMU_RESET_SOURCE());

    u8 reg;
    reg = get_wake_up_type();
    otp_printf("reg0 : %x\n", reg);

    reg = check_io_wakeup_pend();
    otp_printf("reg1 : %x\n", reg);
#endif

    puts(__DATE__);
    puts(__TIME__);
    putchar('\n');

    power_init_app(PWR_MODE_SELECT, otp_arg_list.chargeV);

    putchar('2');

    memory_init(cfg_addr);
    putchar('3');
    /* close_wdt(); */
    open_wdt(WD_8S);			//set wd 32s

    putchar('4');
    //msg_pool_init
    task_message_init();

    putchar('5');
    //task_run_start
    irq_global_enable();

    putchar('6');
    board_init();

    putchar('7');
    /* mcpwm_demo(); */
    /* rtc_test_demo(); */
    /* pdm_link_demo(); */

    /* lowpower_test_demo(); */
    /* nv_mem_test_demo(); */
}

void *task_bt_init(void *priv);
void task_bt_deal(void *hdl);
void task_run(void)
{
    puts("task_run\n");

    task_switch(TASK_ID_BT, NULL);
    /* task_switch(TASK_ID_IDLE, NULL); */
    /* task_bt_init(NULL); */


    while (1) {
        /* task_bt_deal(NULL); */
        task_manager();
    }
}


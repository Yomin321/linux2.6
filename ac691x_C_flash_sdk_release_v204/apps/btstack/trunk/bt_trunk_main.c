#include "bt_trunk_main.h"
#include "bluetooth/avctp_user.h"
#include "sdk_cfg.h"
#include "clock.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".bt_app_bss")
#pragma data_seg(	".bt_app_data")
#pragma const_seg(	".bt_app_const")
#pragma code_seg(	".bt_app_code")
#endif

/*蓝牙音乐、通话、提示音复用buf*/
static u8 bt_sbc_buf[13 * 1024] sec_used(.bt_sbc)__attribute__((aligned(4)));
/*蓝牙协议变量使用*/
static u8 bt_stack_mem[20 * 1024 + 512] sec_used(.bt_stack)__attribute__((aligned(4)));
/*蓝牙发送buf*/
static u8 tx_mem[2 * 1024] sec(.bt_classic_data)__attribute__((aligned(4)));

QLZ(.qlz_init)
void bt_app_cfg()
{
    bt_config_default_init();
    bt_mask_callback_init();

    if (!bt_stack_all_mem_init(bt_stack_mem, sizeof(bt_stack_mem), bt_sbc_buf, sizeof(bt_sbc_buf))) {
        puts("bt mem not enouth \n");
        while (1);
    }

    cfg_test_box_and_work_mode(NON_TEST, BT_MODE);
    cfg_ble_work_mode(BT_MODE);
    /* #if (BT_MODE == TEST_BQB_MODE) */
    /* ble_RF_test_uart_para_init(UART2_HARDWARE_NAME,57600,UART_TXPC4_RXPC5); */
    /* #endif */
    cfg_bt_pll_para(OSC_Hz, SYS_Hz, BT_ANALOG_CFG, BT_XOSC);
    cfg_bredr_mem(NULL, 0, tx_mem, sizeof(tx_mem));

    __set_ble_bredr_mode(BLE_BREDR_MODE);    /*bt enble BT_BLE_EN|BT_BREDR_EN */

    bt_function_select_init();

#if (BLE_BREDR_MODE&BT_BREDR_EN)
    bredr_handle_register();
#endif   //SUPPORT BREDR
#if (BLE_BREDR_MODE&BT_BLE_EN)
    register_ble_init_handle();
    //app_server_init();
#endif   //SUPPORT BLE
}


QLZ(.qlz_init)
void bt_mode_init()
{
    bt_app_cfg();
    bt_stack_init(NULL);
}


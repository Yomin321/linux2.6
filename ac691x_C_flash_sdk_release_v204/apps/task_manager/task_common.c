#include "sdk_cfg.h"
#include "common/common.h"
#include "task_common.h"
#include "task_manager.h"
#include "uart.h"
#include "msg.h"
#include "bluetooth/avctp_user.h"
#include "dec/warning_tone.h"
#include "audio/dac_api.h"
#include "audio/dac.h"
#include "audio/tone.h"
#include "audio/eq.h"
#include "wdt.h"
#include "led.h"
#include "audio/tone.h"
#include "nv_mem.h"
#include "memory_api.h"
#include "dev_manage.h"
#include "timer.h"
#include "power.h"
#include "flash_api.h"
/* #include "echo_api.h" */
#include "music_player.h"
#include "task_manager.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".task_common_bss")
#pragma data_seg(	".task_common_data")
#pragma const_seg(	".task_common_const")
#pragma code_seg(	".task_common_code")
#endif
u8 test_box_vol_up = 0x41;
u8 test_box_vol_down = 0x42;

u32 powerup_dev_detect_ignore(u32 msg)
{
    if (dev_delay_10ms() > 0) {
        switch (msg) {
        case MSG_SD0_ONLINE:
            dev_online_mount(sd0);
            dev_power_off_spec_dev(sd0);
            return NO_MSG;

        case MSG_SD1_ONLINE:
            dev_online_mount(sd1);
            dev_power_off_spec_dev(sd1);
            return NO_MSG;

        case MSG_HUSB_ONLINE:
            dev_online_mount(usb);
            dev_power_off_spec_dev(usb);
            return NO_MSG;
        case MSG_PC_ONLINE:
            return NO_MSG;
        case MSG_AUX_ONLINE:
            return NO_MSG;
        default:
            break;
        }
    }

    return msg;
}

static u32 input_number;	    ///<输入的数字的实际值
static u8 input_number_cnt;   //输入数字个数
static u8 input_number_time_cnt;   //输入数字超时计数
void input_number_deal(u32 num)
{
    if (num < 10) {
        input_number = input_number * 10 + num;
        input_number_cnt++;
        input_number_time_cnt = 0;
        if ((input_number > 9999) || (input_number_cnt > 4)) {
            input_number = num;
            input_number_cnt = 1;
        }
        /* UI_menu(MENU_INPUT_NUMBER, input_number | input_number_cnt << 24, 0); */
        printf("input_number = %u   time_cnt = %d\n", input_number, input_number_time_cnt);
    } else {
        if (input_number_cnt) {
            if (input_number_time_cnt < 3) {
                input_number_time_cnt++;
                if (input_number_time_cnt == 3) {
                    task_post_msg(NULL, 1, MSG_INPUT_TIMEOUT);
                    /* UI_DIS_MAIN(); */
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  获取IR输入数字值
   @param  函数会将input_num通过该指针返回
   @return 输入数字个数
   @note   input_cnt = get_input_number(&input_num)
*/
/*----------------------------------------------------------------------------*/
u32 get_input_number(u32 *num)
{
    u32 ret = input_number_cnt;
    *num = input_number;
    input_number_cnt = 0;
    input_number = 0;
    return ret;
}


u32 powerup_ignore_some_msg(u32 msg)
{
    switch (msg) {
    case MSG_CHANGE_WORKMODE:
    case MSG_POWER_OFF:
    case MSG_POWER_OFF_HOLD:
    case MSG_POWER_KEY_UP:
        return NO_MSG;
    }
    return msg;
}

/*----------------------------------------------------------------------------*/
/**@brief  tbool task_common_msg_deal(void *hdl, u32 msg)
  @param
  @return
   @note
 */
/*----------------------------------------------------------------------------*/
u8 eq_mode = 0;
extern void resource_manage_schedule();
tbool task_common_msg_deal(void *hdl, u32 msg)
{
    tbool exit = true;
    static u8 power_off_doing = 0;
    static u8 power_off_cnt = 0;
    resource_manage_schedule();
#if !POWERUP_DET_EN
    msg = powerup_dev_detect_ignore(msg);
#endif

    if (control_power_on_cnt(3, 0)) {
        /* puts("not power_on\n"); */
        msg = powerup_ignore_some_msg(msg);
    }
    if (msg == NO_MSG) {
        return exit;
    }


    switch (msg) {
    case MSG_CHANGE_WORKMODE:
        puts("---------------task_switch---------------\n");
        if (task_switch(TASK_ID_TYPE_NEXT, NULL) == true) {
            exit = false;
        }
        break;
    case MSG_SD0_ONLINE:
        puts("MSG_SD0_ONLINE\n");
        if (dev_online_mount(sd0)) {
            if (task_get_cur() == TASK_ID_MUSIC) {           //当前为音乐任务不切换,把该消息给music任务处理
                puts("MUSIC :MSG_SD0_ONLINE_SUCC\n");
                task_post_event(NULL, 1, EVENT_SD0_ON_SUCC);
                break;
            }

            if (task_switch(TASK_ID_MUSIC, sd0) == true) {
                exit = false;
            }
        }
        break;

    case MSG_SD0_OFFLINE:
        puts("MSG_SD0_OFFLINE\n");
        dev_offline_unmount(sd0);
        break;

    case MSG_SD1_ONLINE:
        puts("MSG_SD1_ONLINE\n");
        if (dev_online_mount(sd1)) {
            if (task_get_cur() == TASK_ID_MUSIC) {           //当前为音乐任务不切换,把该消息给music任务处理
                puts("MUSIC :MSG_SD1_ONLINE_SUCC\n");
                task_post_event(NULL, 1, EVENT_SD1_ON_SUCC);
                break;
            }

            if (task_switch(TASK_ID_MUSIC, sd1) == true) {
                exit = false;
            }
        }
        break;

    case MSG_SD1_OFFLINE:
        puts("MSG_SD1_OFFLINE\n");
        dev_offline_unmount(sd1);
        break;

    case MSG_HUSB_ONLINE:
        puts("MSG_HUSB_ONLINE\n");
        if (dev_online_mount(usb)) {
            if (task_get_cur() == TASK_ID_MUSIC) {           //当前为音乐任务不切换,把该消息给music任务处理
                puts("MUSIC :MSG_HUSB_ONLINE_SUCC\n");
                task_post_event(NULL, 1, EVENT_USB_ON_SUCC);
                break;
            }

            if (task_switch(TASK_ID_MUSIC, usb) == true) {
                puts("usb music task false\n");
                exit = false;
            }
        }
        break;

    case MSG_HUSB_OFFLINE:
        puts("MSG_HUSB_OFFLINE\n");
        dev_offline_unmount(usb);
        break;

    case MSG_PC_ONLINE:
        puts("MSG_PC_ONLINE\n");
#if USB_PC_EN
        if (task_switch(TASK_ID_PC, NULL) == true) {
            exit = false;
        }
#endif
        break;

#if LINEIN_EN
    case MSG_AUX_ONLINE:
        puts("MSG_AUX_IN\n");
        if (task_switch(TASK_ID_LINEIN, NULL) == true) {
            exit = false;
        }
        break;

    case MSG_AUX_OFFLINE:
        puts("MSG_AUX_OUT\n");
        if (task_get_cur() == TASK_ID_LINEIN) {
            if (task_switch(TASK_ID_TYPE_NEXT, NULL) == true) {
                exit = false;
            }
        }
        break;
#endif

    case MSG_VOL_UP_TWS:
    case MSG_VOL_UP:
        dac_ctl.sys_vol_l++;

        if (dac_ctl.sys_vol_l > get_max_sys_vol(0)) {
            dac_ctl.sys_vol_l = get_max_sys_vol(0);
            /* if (get_tone_status() == 0) { */
            /* warning_tone_play(TONE_POWER_ON, 1); */
            /* } */
            tone_play(500);
        }

        user_send_cmd_prepare(USER_CTRL_TEST_KEY, 1, &test_box_vol_up);
        otp_printf("VOL+:%d\n", dac_ctl.sys_vol_l);
        dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
        set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
        bt_vol_set_memory(&dac_ctl.sys_vol_l, NV_MEM_SYS_VOL, 1);
        user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_SEND_VOL, 0, NULL);
        if (get_call_status() != BT_CALL_HANGUP) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_VOLUME_UP, 0, NULL);
        }
        break;

    case MSG_VOL_DN_TWS:
    case MSG_VOL_DOWN:
        if (dac_ctl.sys_vol_l) {
            dac_ctl.sys_vol_l--;
        }
        user_send_cmd_prepare(USER_CTRL_TEST_KEY, 1, &test_box_vol_down);
        otp_printf("VOL-:%d\n", dac_ctl.sys_vol_l);
        dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
        set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
        bt_vol_set_memory(&dac_ctl.sys_vol_l, NV_MEM_SYS_VOL, 1);
        user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_SEND_VOL, 0, NULL);
        if (get_call_status() != BT_CALL_HANGUP) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_VOLUME_DOWN, 0, NULL);
        }
        break;
    case MSG_VOL_KEY_UP:
        puts("MSG_VOL_KEY_UP\n");
        music_tone_stop();
        delay_2ms(45);
        break;
    case MSG_0:
    case MSG_1:
    case MSG_2:
    case MSG_3:
    case MSG_4:
    case MSG_5:
    case MSG_6:
    case MSG_7:
    case MSG_8:
    case MSG_9:
    case MSG_ONE_SECOND:
        vm_cache_submit();
        input_number_deal(msg);
        /* UI_REFRESH(MENU_SEC_REFRESH); */
        break;

    case MSG_LOW_POWER_VOICE:
        otp_printf("MSG_LOW_POWER_VOICE\n");
        /* tone_play(TONE_WARNING, 0); */
        tone_play(500);
        led_fre_set(C_RLED_LOWER_POWER_MODE);
        break;
    case MSG_LOW_POWER:
    case MSG_POWER_OFF_AUTO:
        otp_printf("MSG_POWER_OFF_AUTO\n");
        if (task_get_cur() != TASK_ID_IDLE) {
            if (task_switch(TASK_ID_IDLE, NULL) == true) {
                /* set_r_led_on_cnt(3); */
                exit = false;
            }
        }
        break;
    case MSG_POWER_OFF:
        puts("MSG_POWER_OFF\n");
        set_going_to_pwr_off(GOINT_POWER_OFF_START_CNT);
        break;

    case MSG_POWER_OFF_HOLD:
        puts("MSG_POWER_OFF_HOLD\n");
        if (get_going_to_pwr_off() && power_off_doing == 0) {
            if (power_off_cnt >= GOINT_POWER_OFF_END_CNT) {
                power_off_doing = 1;
                set_led_scan(0);
            }
            power_off_cnt++;
        }
        if (power_off_doing) {
            power_off_doing++;
            if (power_off_doing > 2) {
                power_off_doing = 0;
                task_post_msg(NULL, 1, MSG_POWER_KEY_UP);
            }
        }
        break;
    case MSG_POWER_KEY_UP:
        puts("MSG_POWER_KEY_UP\n");
        if (power_off_cnt >= GOINT_POWER_OFF_END_CNT) {
            set_r_led_on_cnt(3);
            if (task_switch(TASK_ID_IDLE, NULL) == true) {
                exit = false;
            }
            break;
        }
        set_led_scan(1);
        set_going_to_pwr_off(0);
        power_off_cnt = 0;
        power_off_doing = 0;
        break;
    case MSG_MUTE:
        puts("MSG_MUTE\n");
        if (is_dac_mute()) {
            dac_mute(0, 1);
        } else {
            dac_mute(1, 1);
        }
        break;
    case MSG_EQ_MODE_TWS:
    case MSG_EQ_MODE:
        eq_mode++;
        if (eq_mode > EQ_MODE_MAX) {
            eq_mode = 0;
        }
        otp_printf("eq_mode:%d\n", eq_mode);
        eq_mode_set(eq_mode);
        break;
    case MSG_BACK_TO_BT:
        if (task_switch(TASK_ID_BT, NULL) == true) {
            exit = false;
        }
        break;
    case MSG_LAST_WORKMOD:
        if (task_switch(TASK_ID_TYPE_LAST, NULL) == true) {
            exit = false;
        }
        break;

    default:
        break;
    }
    return exit;
}


#include "task_bt.h"
#include "task_bt_key.h"
#include "msg.h"
#include "task_manager.h"
#include "task_common.h"
#include "bt_trunk_main.h"
#include "bluetooth/avctp_user.h"
#include "aec_main.h"
#include "uart.h"
#include "audio/dac_api.h"
#include "audio/dac.h"
#include "string.h"
#include "sdk_cfg.h"
/* #include "bt_key_msg.h" */
/* #include "key_drv/key.h" */
/* #include "sys_detect.h" */
#include "common/sys_timer.h"
//#include "bt_tone.h"
#include "dec/warning_tone.h"
#include "led.h"
#include "compress.h"
#include "charge.h"
#include "power/power.h"
#include "tone.h"
#include "rtc/rtc_api.h"
#include "charge.h"
#include "power_manage_api.h"
#include "flash_api.h"
#include "music_player.h"
#include "updata.h"
#include "clock.h"


#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".task_bt_bss")
#pragma data_seg(	".task_bt_data")
#pragma const_seg(	".task_bt_const")
#pragma code_seg(	".task_bt_code")
#endif



/* #define TASK_BT_DEBUG_ENABLE */
#ifdef TASK_BT_DEBUG_ENABLE
#define task_bt_printf otp_printf
#define task_bt_puts puts
#else
#define task_bt_printf(...)
#define task_bt_puts(...)
#endif// TASK_BT_DEBUG_ENABLE

struct user_ctrl {
    u8 phone_num_len;
    u8 income_phone_num[30];
    u8 auto_connection_counter;
    u8 connect_addr[6];
    u8 phone_ring_flag;
    u8 phone_num_flag;
    u8 phone_num_idx;
    u16 auto_shutdown_cnt;
    u8 last_call_type;
    struct sys_timer bt_connect_timeout;
    struct sys_timer bt_prompt_timeout;
    u8 going_to_pwr_off;
    u16 power_on_cnt;
    u8 update_name_end;
};

static struct user_ctrl __user_val sec_used(.ram1_bss);
#define user_val (&__user_val)

extern u8 a2dp_get_status(void);
extern void resource_manage_schedule();
extern u8 max_pwr;

u8 get_going_to_pwr_off()
{
    return (user_val->going_to_pwr_off);
}

void set_going_to_pwr_off(u8 cnt)
{
    user_val->going_to_pwr_off = cnt;
}
u16 control_power_on_cnt(u8 mode, u16 poweron_cnt)
{
    if (POWER_ON_CNT_SET == mode) {
        user_val->power_on_cnt = poweron_cnt;
    } else if (POWER_ON_CNT_INC == mode) {
        if (user_val->power_on_cnt) {
            user_val->power_on_cnt--;
        }
    }
    return user_val->power_on_cnt;
}
void power_on_ignore_msg(void)
{
    control_power_on_cnt(POWER_ON_CNT_SET, 4);
}
void bt_msg_power_off()
{
    if (!get_ldo5v_online_flag()) {
        set_going_to_pwr_off(GOINT_POWER_OFF_START);
        task_post_msg(NULL, 1, MSG_POWER_OFF_AUTO);
    }
}
/*获取到来电电话号码，用于回拨功能*/
void hook_hfp_incoming_phone_number(char *number, u16 length)
{
    user_val->phone_num_len = length;
    //printf("%x",sizeof(user_val->income_phone_num));

    if (length <= sizeof(user_val->income_phone_num)) {
        memcpy(user_val->income_phone_num, number, length);
    }
    if (user_val->phone_num_len) {
        user_val->phone_num_flag = 1;
        puts("phone_num:");
        put_buf(user_val->income_phone_num, user_val->phone_num_len);
    }

    /* put_buf(user_val->income_phone_num,user_val->phone_num_len); */
}

/*开关可发现可连接的函数接口*/
static void bt_page_scan(u8 enble)
{
    if (enble) {
        if (!is_1t2_connection()) {
            if (get_remote_test_flag() && get_total_connect_dev()) {
                user_send_cmd_prepare(USER_CTRL_WRITE_SCAN_DISABLE, 0, NULL);
                user_send_cmd_prepare(USER_CTRL_WRITE_CONN_DISABLE, 0, NULL);
            } else {
                user_send_cmd_prepare(USER_CTRL_WRITE_SCAN_ENABLE, 0, NULL);
                user_send_cmd_prepare(USER_CTRL_WRITE_CONN_ENABLE, 0, NULL);
            }
        }
    } else {
        user_send_cmd_prepare(USER_CTRL_WRITE_SCAN_DISABLE, 0, NULL);
        user_send_cmd_prepare(USER_CTRL_WRITE_CONN_DISABLE, 0, NULL);
    }
}
void bt_update_name_end()
{
    if (user_val->update_name_end == 0xaa) {
        puts("bt_update_name_end\n");
        bt_page_scan(0);
        set_sys_vol(SYS_DEFAULT_VOL, SYS_DEFAULT_VOL, FADE_OFF);
        tone_play(500);
        led_fre_set(C_ALL_ON_MODE);

    }

}
void phone_ring_play(struct sys_timer *ts)
{
    if (get_cur_conn_support_inband_ringtone()) {
        task_bt_puts("support_inband_ringtone\n");
        return;
    }

    if (user_val->phone_ring_flag) {

        task_post_msg(NULL, 1, MSG_BT_TONE_RING);
        sys_timer_reset(ts, 4000);
    }
}
void phone_num_play()
{
    /*play 0~9*/
    if ((user_val->income_phone_num[user_val->phone_num_idx] >= 0x30) &&
        (user_val->income_phone_num[user_val->phone_num_idx] <= 0x39)) {
        if (user_val->phone_num_idx == (user_val->phone_num_len - 1)) {
            warning_tone_play(TONE_NUM_0 + user_val->income_phone_num[user_val->phone_num_idx] - 0x30, 0);
        } else {
            warning_tone_play(TONE_NUM_0 + user_val->income_phone_num[user_val->phone_num_idx] - 0x30, 1);
        }
    }
    user_val->phone_num_idx++;
    if (user_val->phone_num_idx >= user_val->phone_num_len) {
        user_val->phone_num_idx = 0;;
        sys_timer_remove(&user_val->bt_prompt_timeout);
        if (!get_cur_conn_support_inband_ringtone()) {
            /*
             *报号完接着播来电提示音
             *这个定时如果过长，同时又支持inband_ringtone，就会先播一下inband_ringtone
             */
            sys_timer_register(&user_val->bt_prompt_timeout, 1500, phone_ring_play, 1);
        }
    }
}
void phone_num_play_timer(struct sys_timer *ts)
{
    if (user_val->phone_num_flag) {
        if (user_val->phone_num_idx < user_val->phone_num_len) {
            task_post_msg(NULL, 1, MSG_BT_TONE_PHONE_NUM);
            sys_timer_reset(ts, 1000);
        }
    } else {
        /*电话号码还没有获取到，定时查询*/
        sys_timer_reset(ts, 200);
    }
}

void phone_num_play_start()
{
    user_val->phone_num_flag = 0;
    user_val->phone_num_idx = 0;
    sys_timer_register(&user_val->bt_prompt_timeout, 500, phone_num_play_timer, 1);
}

/*协议栈状态变化用户处理接口*/
int btstack_status_update_deal(u8 *info, u16 len)
{
    u8 status;
    //put_buf(info, len);
    status = info[3];

    switch (status) {
    case BT_STATUS_INIT_OK:
        task_bt_puts("BT_STATUS_INIT_OK\n");
        led_fre_set(C_RB_FAST_MODE);


#if (BT_MODE != NORMAL_MODE)
        bt_page_scan(1);
#endif
        //user_send_cmd_prepare(USER_CTRL_START_CONNECTION, 0, NULL);
        //  u8 temp_addr[6] = {0x9c, 0xfb, 0xd5, 0xe0, 0x9b, 0xd6};
        /* user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR, 6, temp_addr); */

        break;

    case BT_STATUS_FIRST_CONNECTED:
    case BT_STATUS_SECOND_CONNECTED:
        if (is_1t2_connection()) {
            __set_max_pwr(max_pwr);
        }
        led_fre_set(C_BLED_FAST_ONE_5S_MODE);
        /* led_fre_set(C_BLED_FAST_DOBLE_5S_MODE); */
        task_post_msg(NULL, 1, MSG_BT_TONE_CONN);
        break;
    case BT_STATUS_FIRST_DISCONNECT:
    case BT_STATUS_SECOND_DISCONNECT:
        task_post_msg(NULL, 1, MSG_BT_TONE_DISCONN);
        break;

    case BT_STATUS_PHONE_INCOME:

        user_val->phone_ring_flag = 1;
#if BT_PHONE_NUMBER
        phone_num_play_start();
#else
        sys_timer_register(&user_val->bt_connect_timeout, 500, phone_ring_play, 1);
#endif
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_CURRENT, 0, NULL); //发命令获取电话号码
        task_bt_puts("phone_income\n");
        break;
    case BT_STATUS_PHONE_OUT:

        user_send_cmd_prepare(USER_CTRL_HFP_CALL_CURRENT, 0, NULL); //发命令获取电话号码
        task_bt_puts("phone_out\n");
        break;

    case BT_STATUS_PHONE_ACTIVE:
        if (1 == user_val->phone_ring_flag) {
            user_val->phone_ring_flag = 0;
            mutex_resource_release("tone");
            sys_timer_remove(&user_val->bt_connect_timeout);
            sys_timer_remove(&user_val->bt_prompt_timeout);
        }
        break;
    case BT_STATUS_PHONE_HANGUP:
        if (1 == user_val->phone_ring_flag) {
            user_val->phone_ring_flag = 0;
            mutex_resource_release("tone");
            sys_timer_remove(&user_val->bt_connect_timeout);
            sys_timer_remove(&user_val->bt_prompt_timeout);
        }
        break;
    case BT_STATUS_RECONN_OR_CONN:
        if (info[4]) {
            task_bt_puts("reconn ok\n");
        } else {
            task_bt_puts("phone conn ok\n");
        }
        break;
    case BT_STATUS_SNIFF_DAC_CTL:
        otp_printf("BT_STATUS_SNIFF_DAC_CTL:%d\n", info[4]);
        if (info[4]) {
            task_post_msg(NULL, 1, MSG_DAC_ON);
        } else {
            task_post_msg(NULL, 1, MSG_DAC_OFF);
        }
        break;
    case BT_STATUS_LAST_CALL_TYPE_CHANGE:
        otp_printf("BT_STATUS_LAST_CALL_TYPE_CHANGE:%d\n", info[4]);
        user_val->last_call_type = info[4];
        break;
    case BT_STATUS_CALL_VOL_CHANGE:
        dac_ctl.sys_vol_l = (info[4] * aec_param.dac_analog_gain / 15) ;
        otp_printf("VOL:%d\n", dac_ctl.sys_vol_l);
        dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
        set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
        break;
    case BT_STATUS_PHONE_NUMBER:
        otp_printf("BT_STATUS_PHONE_NUMBER\n");
        hook_hfp_incoming_phone_number((char *)&info[6], info[5]);
        break;
    default:
        break;
    }
    return 0;
}


static void sys_time_auto_connection_deal(struct sys_timer *ts)
{
    if (user_val->auto_connection_counter && get_call_status() == BT_CALL_HANGUP) {
        bt_page_scan(0);
        otp_printf("auto_conn_cnt:%d\n", user_val->auto_connection_counter);
        user_val->auto_connection_counter--;
        clear_led_rb_flag();
        user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR, 6, user_val->connect_addr);
    }
}
static void sys_timer_auto_connect(u8 *addr)
{
    memcpy(user_val->connect_addr, addr, 6);
    sys_timer_register(&user_val->bt_connect_timeout, 5000, sys_time_auto_connection_deal, 1);
}

/*不属于用户接口，协议栈回调函数*/
void bt_discon_complete_handle(u8 *addr, int reason)
{
    u8 cur_search_index = 0;
    cur_search_index = get_current_search_index();
    otp_printf("bt_discon_complete:0x%x\n", reason);
    otp_printf("connect_dev=0x%x\n", user_get_total_connect_dev());
    otp_printf("cur_search_index=0x%x\n", cur_search_index);
    if ((user_get_total_connect_dev() == 0) && ((cur_search_index == 0) || cur_search_index == 0xff)) {
        if (get_updata_end_flag() == UPDATA_OTHER) {
            bt_update_name_end();
            return;
        } else {
            led_fre_set(C_RB_FAST_MODE);
        }
    }
    if (reason == 0 || reason == 0x40) {
        //连接成功
        if (reason == 0) {
            user_val->auto_connection_counter = 0;
            sys_timer_remove(&user_val->bt_connect_timeout);
        }
        return ;
    } else if (reason == 0xfc) {
        //新程序没有记忆地址是无法发起回连
        bt_page_scan(1);
        __set_max_pwr(max_pwr);
        return ;
    } else if ((reason == 0x10) || (reason == 0xf)) {
        task_bt_puts("conneciton accept timeout\n");
        bt_page_scan(1);
        return ;
    }

    if (reason == 0x16 || reason == 0x13) {
        task_bt_puts("Conn Terminated by Local Host\n");
        bt_page_scan(1);
    } else if (reason == 0x08) {
        task_bt_puts("\nconntime timeout\n");
        if (!get_remote_test_flag()) {
            if (get_call_status() == BT_CALL_HANGUP) {
                user_val->auto_connection_counter = 6;
                task_bt_puts("\nsuper timeout\n");
                sys_timer_remove(&user_val->bt_connect_timeout);
                clear_led_rb_flag();
                user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR, 6, addr);
            }
        } else {
            user_val->auto_connection_counter = 0;
            bt_page_scan(1);
        }
    } else if (reason == 0x04) {
        if (! user_val->auto_connection_counter) {
            puts("page timeout---\n");
            if (get_current_search_index() >= 1) {
                //继续搜索下一个设备
                user_send_cmd_prepare(USER_CTRL_START_CONNECTION, 0, NULL);
            } else {
                bt_page_scan(1);
                __set_max_pwr(max_pwr);
            }
        } else {
            user_val->auto_connection_counter--;
            if (user_val->auto_connection_counter % 2) {
                bt_page_scan(1);
                sys_timer_auto_connect(addr);
            } else {
                bt_page_scan(0);
                clear_led_rb_flag();
                user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR, 6, user_val->connect_addr);
            }
        }
    } else if (reason == 0x0b) {
        task_bt_puts("Connection Exist\n");
        clear_led_rb_flag();
        user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR, 6, addr);
    } else if (reason == 0x06) {
        //connect continue after link missing
        // user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR, 6, addr);
    } else if ((reason == 0x0d) || (reason == 0x05) || (reason == 0x0A)) {
        /*
         *reason:
         *0x05:Authentication Fdilure
         *0x0A:Limit to a Device Exceeded
         *0x0D:Limit Resources
         */
        puts("connection rejected due to limited resources\n");
        bt_page_scan(1);
    }
}


void bt_test_fun()
{
    user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR_MANUALLY, 0, NULL);
}

void paly_powr_on_tone()
{
    warning_tone_play(TONE_POWER_ON, 0);
#if CHECK_PULL_UP_EN
    while (1) {
        resource_manage_schedule();
        clear_wdt();
        if ((!IS_POWER_KEY_DOWN()) && (bt_tone_get_status() == 0)) {
            check_pull_up_value();
            break;
        }
        putchar('k');
    }
#endif
}

QLZ(.qlz_init)
void *task_bt_init(void *priv)
{
    set_sys_freq(BT_SYS_Hz);
    otp_printf("task_bt_init !!\n");
    user_val->last_call_type = BT_CALL_OUTGOING;
    dac_toggle(1);
    key_msg_reg(&task_bt_key);
    /* resource_manage_init_app(); */
#if (SNIFF_MODE_CONF&SNIFF_EN)
    led_bt_sniff_init();
#endif
    led_fre_set(C_RB_FAST_MODE);
    dac_channel_on(BT_CHANNEL, FADE_ON);
    paly_powr_on_tone();
    bt_mode_init();
    bt_osc_init();

    return NULL;
}
QLZ(.qlz_comm)
extern void a2dp_media_stop(void **hdl);
static void task_bt_exit(void **hdl)
{
    otp_printf("task_bt_exit !!\n");
    dac_toggle(1);
    a2dp_media_stop(*hdl);
    no_background_suspend();
    task_clear_all_message();
}

void task_bt_deal(void *hdl)
/* void task_bt_deal(void) */
{
    int msg[2];
    int error = MSG_NO_ERROR;
    /* u8 power_off_doing = 0; */
    u8 power_off_cnt = 0;
    /* #if CHECK_PULL_UP_EN */
    /* control_power_on_cnt(POWER_ON_CNT_SET, 0); */
    /* #else */
    /* control_power_on_cnt(POWER_ON_CNT_SET, 4); */
    /* #endif */
    printf("--------------BTBTBTBTBTBT----------task_bt_deal !!\n");
    bank_switch(QLZ_COMM);

#if (BT_MODE == NORMAL_MODE)
    user_send_cmd_prepare(USER_CTRL_START_CONNECTION, 0, NULL);
#endif

#if (TASK_MANGER_ENABLE == 0)
    task_bt_init(NULL);
#endif

    u32 loop_cnt = 0;
    user_val->auto_shutdown_cnt = AUTO_SHUT_DOWN_TIME;
    while (1) {
        loop_cnt++;
        if (loop_cnt > 300000) {
            putchar('<');
        }
        aec_task_main();
        if (loop_cnt > 300000) {
            putchar('M');
        }
        resource_manage_schedule();
        if (loop_cnt > 300000) {
            loop_cnt = 0;
            putchar('>');
        }
        clear_wdt();
        error = task_get_msg(0, 2, msg);
        if (task_common_msg_deal(NULL, msg[0]) == false) {
            music_tone_stop();
            task_common_msg_deal(NULL, NO_MSG);
            return ;
        }
        if (NO_MSG == msg[0]) {
            continue;
        }

        switch (msg[0]) {
        case SYS_EVENT_PLAY_SEL_END:
            puts("SYS_EVENT_PLAY_SEL_END\n");
            break;
        case MSG_BT_PP:
            puts("MSG_BT_PP\n");
            task_bt_printf("call_status:%d\n", get_call_status());
            if ((get_call_status() == BT_CALL_OUTGOING) ||
                (get_call_status() == BT_CALL_ALERT)) {
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
            } else if (get_call_status() == BT_CALL_INCOMING) {
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_ANSWER, 0, NULL);
            } else if (get_call_status() == BT_CALL_ACTIVE) {
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
                /* user_send_cmd_prepare(USER_CTRL_SCO_LINK, 0, NULL); */
            } else {
                user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PLAY, 0, NULL);
            }
            break;

        case MSG_BT_NEXT_FILE:
            puts("MSG_BT_NEXT_FILE\n");
            if (get_call_status() == BT_CALL_ACTIVE) {
                /* user_send_cmd_prepare(USER_CTRL_HFP_CALL_VOLUME_UP, 0, NULL); */
            } else {
                user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_NEXT, 0, NULL);
            }

            break;

        case MSG_BT_PREV_FILE:
            puts("MSG_BT_PREV_FILE\n");
            task_bt_puts("MSG_BT_PREV_FILE\n");
            if (get_call_status() == BT_CALL_ACTIVE) {
                /* user_send_cmd_prepare(USER_CTRL_HFP_CALL_VOLUME_DOWN, 0, NULL); */
            } else {
                user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PREV, 0, NULL);
            }
            break;
        case MSG_BT_HID_TAKE_PIC:
            if (get_curr_channel_state()&HID_CH) {
                puts("---USER_CTRL_HID_IOS\n");
                user_send_cmd_prepare(USER_CTRL_HID_IOS, 0, NULL);


            }
            break;
        case MSG_BT_CALL_LAST_NO:
            task_bt_puts("MSG_BT_CALL_LAST_NO\n");
            if (get_call_status() != BT_CALL_HANGUP) {
                break;
            }

            if (user_val->last_call_type == BT_CALL_OUTGOING) {
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_LAST_NO, 0, NULL);
            } else if (user_val->last_call_type == BT_CALL_INCOMING) {
                user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER, user_val->phone_num_len,
                                      user_val->income_phone_num);
            }
            break;
        case MSG_BT_CALL_REJECT:
            task_bt_puts("MSG_BT_CALL_REJECT\n");
            if ((get_call_status() == BT_CALL_OUTGOING) ||
                (get_call_status() == BT_CALL_ALERT)) {
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
            } else if (get_call_status() == BT_CALL_INCOMING) {
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_ANSWER, 0, NULL);
            } else if (get_call_status() == BT_CALL_ACTIVE) {
                user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
                /*user_send_cmd_prepare(USER_CTRL_SCO_LINK, 0, NULL); */
            }
            break;
        case MSG_BT_CONNECT_CTL:
            puts("MSG_BT_CONNECT_CTL\n");
            if ((BT_STATUS_CONNECTING == get_bt_connect_status())   ||
                (BT_STATUS_TAKEING_PHONE == get_bt_connect_status()) ||
                (BT_STATUS_PLAYING_MUSIC == get_bt_connect_status())) { /*连接状态*/
                puts("bt_disconnect\n");/*手动断开连接*/
                user_send_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);
            } else {
                puts("bt_connect\n");/*手动连接*/
                user_send_cmd_prepare(USER_CTRL_START_CONNEC_VIA_ADDR_MANUALLY, 0, NULL);
            }
            break;
        case MSG_DAC_ON:
            puts("MSG_DAC_ON\n");
            dac_toggle(1);
            break;
        case MSG_DAC_OFF:
            puts("MSG_DAC_OFF\n");
            dac_toggle(0);
            break;
        case MSG_AUTOMUTE_ON:
            puts("MSG_AUTOMUTE_ON\n");
            dac_automute_set(AUTO_MUTE_CFG, -1, -1, -1); // 开自动mute
            break;
        case MSG_AUTOMUTE_OFF:
            puts("MSG_AUTOMUTE_OFF\n");
            dac_mute(0, 0);
            break;
        case MSG_BT_TONE_CONN:
            puts("MSG_BT_TONE_CONN\n");
            warning_tone_play(TONE_BT_CONN, 0);
            break;
        case MSG_BT_TONE_DISCONN:
            puts("MSG_BT_TONE_DISCONN\n");
            warning_tone_play(TONE_BT_DISCON, 0);
            break;
        case MSG_BT_TONE_PHONE_NUM:
            phone_num_play();
            break;
        case MSG_BT_TONE_RING:
            warning_tone_play(TONE_RING, 1);
            break;
        case MSG_BT_SIRI_OPEN:
            puts("MSG_BT_SIRI_OPEN\n");
            user_send_cmd_prepare(USER_CTRL_HFP_GET_SIRI_OPEN, 0, NULL);
            break;
        case MSG_BT_SIRI_CLOSE:
            puts("MSG_BT_SIRI_CLOSE\n");
            user_send_cmd_prepare(USER_CTRL_HFP_GET_SIRI_CLOSE, 0, NULL);
            break;
#if 0
        case MSG_POWER_OFF:
            puts("MSG_POWER_OFF task_bt\n");
            if (control_power_on_cnt(3, 0)) {
                puts("not power_on\n");
                set_going_to_pwr_off(0);
                break;
            }
            if ((BT_STATUS_CONNECTING == get_bt_connect_status()) ||
                (BT_STATUS_TAKEING_PHONE == get_bt_connect_status()) ||
                (BT_STATUS_PLAYING_MUSIC == get_bt_connect_status())) {
                if (get_call_status() != BT_CALL_HANGUP) {
                    puts("call hangup\n");
                    if (get_going_to_pwr_off() != GOINT_POWER_OFF_START) {
                        user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
                        break;
                    }
                }
            }
            if (get_going_to_pwr_off() == GOINT_POWER_OFF_START) {
                power_off_cnt = GOINT_POWER_OFF_START;
                task_post_msg(NULL, 1, MSG_POWER_OFF_HOLD);
                break;
            }
            set_going_to_pwr_off(GOINT_POWER_OFF_START_CNT);
            break;

        case MSG_POWER_OFF_HOLD:
            puts("MSG_POWER_OFF_HOLD\n");
            if (get_going_to_pwr_off() && power_off_doing == 0) {
                if (power_off_cnt >= GOINT_POWER_OFF_END_CNT) {
                    power_off_doing = 1;
                    set_led_scan(0);
                    warning_tone_play(TONE_POWER_OFF, 0);
                    if (get_going_to_pwr_off() == GOINT_POWER_OFF_START) {
                        task_post_msg(NULL, 1, MSG_POWER_KEY_UP);
                        break;
                    }
                }
                power_off_cnt++;
            }
            if (power_off_doing) {
                power_off_doing++;
                if (power_off_doing > 2) {

                    set_r_led_on_cnt(3);
                    task_post_msg(NULL, 1, MSG_POWER_KEY_UP);
                }
            }
            break;
        case MSG_POWER_KEY_UP:
            puts("MSG_POWER_KEY_UP\n");
            control_power_on_cnt(POWER_ON_CNT_GET, 0);
            if (power_off_cnt > GOINT_POWER_OFF_END_CNT) {
                set_r_led_on_cnt(3);
                while (get_tone_status()) {
                    delay_2ms(1);
                }
                if ((BT_STATUS_CONNECTING == get_bt_connect_status()) ||
                    (BT_STATUS_TAKEING_PHONE == get_bt_connect_status()) ||
                    (BT_STATUS_PLAYING_MUSIC == get_bt_connect_status())) {
                    user_send_cmd_prepare(USER_CTRL_POWER_OFF, 0, NULL);
                }
                dac_toggle(0);
                while (dac_ctl.toggle) {
                    delay_2ms(1);
                }
                delay_2ms(4);
                if (get_ldo5v_online_flag()) {
                    JL_POWER->CON |= BIT(4);
                    while (1);
                } else  {
                    enter_sys_soft_poweroff();

                }
            }
            set_led_scan(1);
            power_off_cnt = 0;
            set_going_to_pwr_off(0);
            power_off_doing = 0;
            break;
#endif
        case MSG_BT_UPDATA_START:
            puts("MSG_BT_UPDATA_START\n");
            updata_mode_api(BT_UPDATA);
            break;

        case MSG_BT_UPDATA_END:
            puts("MSG_BT_UPDATA_END\n");
            user_val->update_name_end = 0xaa;
            break;
        case MSG_HALF_SECOND:
            putchar('H');
            bt_update_name_end();
            if (check_connect_hid()) {
                // puts("USER_CTRL_HID_CONN\n");
                user_send_cmd_prepare(USER_CTRL_HID_CONN, 0, NULL);

            }
            /* task_bt_printf(" BT_H %d \n", get_bt_connect_status()); */

#if (SNIFF_MODE_CONF&SNIFF_EN)
            if (user_sniff_check_req(SNIFF_CNT_TIME)) {
                otp_printf("check_sniff_req\n");
                user_send_cmd_prepare(USER_CTRL_SNIFF_IN, 0, NULL);
            }
#endif

            if ((BT_STATUS_CONNECTING == get_bt_connect_status())   ||
                (BT_STATUS_TAKEING_PHONE == get_bt_connect_status()) ||
                (BT_STATUS_PLAYING_MUSIC == get_bt_connect_status())) { /*连接状态*/

                /* task_bt_printf("S:%d  ", a2dp_get_status()); */

                if (BT_MUSIC_STATUS_STARTING == a2dp_get_status()) {    /*播歌状态*/
                    /* task_bt_puts("bt_music\n"); */
                } else if (BT_STATUS_TAKEING_PHONE == get_bt_connect_status()) {
                    task_bt_puts("bt_phone\n");
                } else {
                }

                user_val->auto_shutdown_cnt = AUTO_SHUT_DOWN_TIME;

            } else  if (BT_STATUS_WAITINT_CONN == get_bt_connect_status() && user_val->auto_shutdown_cnt) {
                //task_bt_puts("BT_STATUS_WAITINT_CONN\n");
                user_val->auto_shutdown_cnt--;
                /* otp_printf("power cnt:%d\n",user_val->auto_shutdown_cnt); */
                if (user_val->auto_shutdown_cnt == 0) {
                    //软关机
                    task_bt_puts("*****shut down*****\n");
                    bt_msg_power_off();
                    /* if (!get_ldo5v_online_flag()) { */
                    /* task_post_msg(NULL, 1, MSG_POWER_OFF_AUTO); */
                    /* } */
                    user_val->auto_shutdown_cnt = AUTO_SHUT_DOWN_TIME;
                    /*
                    if (task_switch(TASK_ID_IDLE, NULL)) {
                        return;
                    }
                    */
                }
            }
            break;

        default:
            break;
        }
    }
}

#if TASK_MANGER_ENABLE
const TASK_APP task_bt_info = {
    .skip_check = NULL,
    .init 		= task_bt_init,
    .exit 		= task_bt_exit,
    .task 		= task_bt_deal,
    .key 		= &task_bt_key,
};
#endif

#include "task_music.h"
#include "task_music_key.h"
#include "msg.h"
#include "string.h"
#include "uart.h"
#include "audio/dac_api.h"
#include "dev_manage.h"
#include "music_player.h"
#include "task_common.h"
#include "dac.h"
#include "updata.h"
#include "dec/warning_tone.h"
#include "fat_io.h"
#include "clock.h"

#define MUSIC_TASK_DEBUG_ENABLE
#ifdef MUSIC_TASK_DEBUG_ENABLE
#define music_task_printf otp_printf
#else
#define music_task_printf(...)
#endif

#include "fs_io.h"

static u8 music_rpt_mode = REPEAT_ALL;
static DEV_HANDLE cur_use_dev = NULL;
static MUSIC_DECODER_ST music_status;

static void music_play_mutex_init(void *priv)
{
    music_task_printf("music_play_mutex_init\n");
    tbool ret;
    MUSIC_PLAYER *obj = priv;

    if (cur_use_dev == (void *)sd0) {
        puts("[PRIV SD0]\n");
    } else if (cur_use_dev == (void *)sd1) {
        puts("[PRIV SD1]\n");
    } else if (cur_use_dev == (void *)usb) {
        puts("[PRIV USB]\n");
    } else {
        puts("[PRIV CACHE]\n");
    }

    dac_channel_on(MUSIC_CHANNEL, 0);
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_ON);
    music_player_creat();

    music_task_printf("music obj:0x%x,dev:0x%x\n", obj, cur_use_dev);
    if ((obj == NULL) || (cur_use_dev == NULL)) {
        music_task_printf("music player play err!!, fun = %s, line = %d\n", __func__, __LINE__);
        task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
    }

    if (music_status == MUSIC_PLAYRR_ST_PAUSE) {
        music_task_printf("music player status!!, fun = %s, line = %d\n", __func__, __LINE__);
        music_player_set_decoder_init_sta(obj, MUSIC_PLAYRR_ST_PAUSE);
    }

    ret = music_player_play_spec_dev(obj, (u32)cur_use_dev);
    if (ret == false) {
        music_task_printf("music player play err!!, fun = %s, line = %d\n", __func__, __LINE__);
        task_post_msg(NULL, 1, MSG_CHANGE_WORKMODE);
    }
}

static void music_play_mutex_stop(void *priv)
{
    music_task_printf("music_play_mutex_stop:0x%x\n", priv);
    MUSIC_PLAYER *obj = priv;
    music_status = music_player_get_status(obj);
    if (obj && cur_use_dev) {
        music_player_destroy(&obj);
    }
}

static MUSIC_PLAYER *music_play_start(void)
{
    MUSIC_PLAYER *obj = NULL;
    obj = music_player_creat();
    music_status = MUSIC_DECODER_ST_PLAY;
    music_task_printf("music_play_creat:0x%x\n", obj);
    if (obj) {
        mutex_resource_apply("music", 3, music_play_mutex_init, music_play_mutex_stop, obj);
    }
    return obj;
}

static tbool task_music_skip_check(void **priv)
{
    music_task_printf("task_music_skip_check !!\n");
    u32 dev_status;

    printf("1music cur_use_dev:0x%x,0x%x\n", cur_use_dev, *priv);
    //check some device online
    if (*priv == NULL) {
        if (cur_use_dev != NULL) {
            if (!dev_get_online_status(cur_use_dev, &dev_status)) {
                if (dev_status != DEV_ONLINE) {                               //上一次退出时的设备不在线，重新获取一个
                    *priv = dev_get_fisrt(MUSIC_DEV_TYPE, DEV_ONLINE);
                } else {                                                      //在线，使用上次保存的设备播放
                    *priv = cur_use_dev;
                }
            }
        } else {
            *priv = dev_get_fisrt(MUSIC_DEV_TYPE, DEV_ONLINE);
        }
        printf("2music cur_use_dev:0x%x,0x%x\n", cur_use_dev, *priv);
        if (*priv == (void *)sd0) {
            puts("[PRIV SD0]\n");
        } else if (*priv == (void *)sd1) {
            puts("[PRIV SD1]\n");
        } else if (*priv == (void *)usb) {
            puts("[PRIV USB]\n");
        } else {
            puts("[PRIV CACHE]\n");
        }
        if (*priv == NULL) {
            return false;
        } else {
            return true;
        }
    }

//check specific device online
    return true;
}

static void music_play_stop(void *priv)
{
    music_tone_stop();
    mutex_resource_release("music");
    task_common_msg_deal(priv, NO_MSG);
}
static void *task_music_init(void *priv)
{
    music_task_printf("task_music_init !!\n");
    tbool ret;
    MUSIC_PLAYER *obj = NULL;

    set_sys_freq(MUSIC_SYS_Hz);
    fat_init();
    dac_channel_on(MUSIC_CHANNEL, 0);
    // set_sys_vol(15, 15, 0);

    obj = music_player_creat();
    cur_use_dev = (DEV_HANDLE)priv;
    warning_tone_play(TONE_MUSIC_MODE, 0);

    return obj;
}

static void task_music_exit(void **hdl)
{
    music_task_printf("task_music_exit !!\n");
#if 1
    /* dac_channel_off(MUSIC_CHANNEL, 0); */
    cur_use_dev = music_player_get_cur_dev(*hdl);
    music_player_destroy((MUSIC_PLAYER **)hdl);
#endif
    task_clear_all_message();
    fat_del();
}

void music_msg_filter(int *msg)
{
    switch (*msg) {
    case MSG_MUSIC_PP:
    case MSG_MUSIC_NEXT_FILE:
    case MSG_MUSIC_PREV_FILE:
    case MSG_MUSIC_FF:
    case MSG_MUSIC_FR:
    case MSG_MUSIC_AB_RPT:
    case MSG_HALF_SECOND:
        *msg = NO_MSG;
        break;
    }
}

static void task_music_deal(void *hdl)
{
    music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
    int msg = NO_MSG;
    int error = MSG_NO_ERROR;
    u32 err;
    tbool ret = true;
    MUSIC_DECODER_ST play_status;
    MUSIC_PLAYER *obj = (MUSIC_PLAYER *)hdl;
    u8 music_start = 0;

    music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
    if (obj == NULL) {
        music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
        ret = false;
    }

    music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
    while (1) {

        error = task_get_msg(0, 1, &msg);
        if (get_tone_status()) {    //提示音还未播完前过滤涉及解码器操作的消息
            music_msg_filter(&msg);
        }
        if (task_common_msg_deal(obj, msg) == false) {
            if (music_player_get_cur_dev(obj) != (void *)cache) {
                cur_use_dev = music_player_get_cur_dev(obj);
            }
            puts("exit_music_task\n");
            music_play_stop(obj);
            return;
        }
        if (NO_MSG == msg) {
            continue;
        }
        if (msg != MSG_HALF_SECOND) {
            /* otp_printf("msg = :%d\n",msg); */
        }
        switch (msg) {
        case SYS_EVENT_DEC_FR_END:
            break;
        case SYS_EVENT_DEC_FF_END:
        case SYS_EVENT_DEC_END:
            music_task_printf("SYS_EVENT_DEC_END, fun = %s, line = %d\n", __func__, __LINE__);
            ret = music_player_operation(obj, PLAY_AUTO_NEXT);
            break;

        case SYS_EVENT_DEC_DEVICE_ERR:
            music_task_printf("SYS_EVENT_DEC_DEVICE_ERR\n");
            ret = music_player_play_next_dev(obj);
            break;

        case SYS_EVENT_PLAY_SEL_END:
            music_task_printf("SYS_EVENT_PLAY_TONE_END\n");
            if (music_start == 0) {
                music_start = 1 ;
                obj = music_play_start();
            }

            break;

        case MSG_MUSIC_AB_RPT:
            music_task_printf("MSG_MUSIC_AB_RPT\n");
            /* music_player_ab_repeat_switch(obj); */
            break;

        case MSG_MUSIC_U_SD:
            music_task_printf("MSG_MUSIC_U_SD\n");
            if (dev_get_phydev_total(MUSIC_DEV_TYPE, DEV_ONLINE) > 1) {
                ret = music_player_play_next_dev(obj);
            }
            break;

        case MSG_SD0_MOUNT_SUCC:
            music_play_stop(NULL);
            cur_use_dev = sd0;
            obj = music_play_start();
            music_start = 1 ;
            break;

        case MSG_SD1_MOUNT_SUCC:
            music_play_stop(NULL);
            cur_use_dev = sd1;
            obj = music_play_start();
            music_start = 1 ;
            break;

        case MSG_USB_MOUNT_SUCC:
            music_play_stop(NULL);
            cur_use_dev = usb;
            obj = music_play_start();
            music_start = 1 ;
            break;

        case MSG_SD0_OFFLINE:
            if (sd0 == music_player_get_cur_dev(obj) && MUSIC_PLAYRR_ST_PAUSE == music_player_get_status(obj)) {  //防止拔出设备时重复推消息
                task_post_msg(NULL, 1, SYS_EVENT_DEC_DEVICE_ERR);
            }
            break;

        case MSG_SD1_OFFLINE:
            if (sd1 == music_player_get_cur_dev(obj) && MUSIC_PLAYRR_ST_PAUSE == music_player_get_status(obj)) {
                task_post_msg(NULL, 1, SYS_EVENT_DEC_DEVICE_ERR);
            }
            break;

        case MSG_HUSB_OFFLINE:
            music_task_printf("music MSG_SUSB_OFFLINE\n");
            if (usb == music_player_get_cur_dev(obj) && MUSIC_PLAYRR_ST_PAUSE == music_player_get_status(obj)) {
                task_post_msg(NULL, 1, SYS_EVENT_DEC_DEVICE_ERR);
            }
            break;

        case MSG_MUSIC_PLAY:
            ret = music_player_play_spec_dev(obj, (u32)cur_use_dev);
            break;

        case MSG_MUSIC_PP:
            music_task_printf("MSG_MUSIC_PP\n");
            play_status = music_player_pp(obj);
            if (play_status == MUSIC_DECODER_ST_PLAY) {
                music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
            } else if (play_status == MUSIC_PLAYRR_ST_PAUSE) {
                music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
            } else {
                music_task_printf("fun = %s, line = %d\n", __func__, __LINE__);
            }
            break;

        case MSG_MUSIC_SPC_FILE:
            ret = music_player_play_spec_file(obj, 1/*example*/);
            break;

        case MSG_MUSIC_NEXT_FILE:
            music_task_printf("MSG_MUSIC_NEXT_FILE\n");
            ret = music_player_operation(obj, PLAY_NEXT_FILE);
            break;
        case MSG_MUSIC_PREV_FILE:
            music_task_printf("MSG_MUSIC_PREV_FILE\n");
            ret = music_player_operation(obj, PLAY_PREV_FILE);
            break;

        case MSG_MUSIC_RPT:
            music_rpt_mode++;
            if (music_rpt_mode >= MAX_PLAY_MODE) {
                music_rpt_mode = REPEAT_ALL;
            }
            music_player_set_repeat_mode(obj, music_rpt_mode);
            break;

        case MSG_MUSIC_FF:
            music_player_ff(obj, 2);
            break;
        case MSG_MUSIC_FR:
            music_player_fr(obj, 2);
            break;
        case MSG_MUSIC_FFR_DONE:
            break;

        case MSG_MUSIC_DEL_FILE:
            music_player_delete_playing_file(obj);
            ///play next
            break;

        case MSG_UPDATA:
            otp_printf("MSG_UPDATA\n");
            device_updata(music_player_get_cur_dev(obj));
            break;

        case MSG_HALF_SECOND:
            putchar('H');
            if (music_player_get_status(obj) != MUSIC_DECODER_ST_STOP) {
                u32 cur_time, total_time;
                cur_time = music_player_get_cur_time(obj);
                total_time = music_player_get_total_time(obj);
                /* music_task_printf("cur_time = %d, total_time = %d\n", cur_time, total_time); */
            }
            break;

        default:
            break;
        }

        if (ret == false) {
            music_task_printf("music player play err!!, fun = %s, line = %d\n", __func__, __LINE__);
            music_play_stop(obj);
            task_switch(TASK_ID_TYPE_NEXT, NULL);
            return ;
        }
    }
}

const TASK_APP task_music_info = {
    .skip_check = task_music_skip_check,
    .init 		= task_music_init,
    .exit 		= task_music_exit,
    .task 		= task_music_deal,
    .key 		= &task_music_key,
};


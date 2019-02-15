#include "task_idle.h"
#include "task_idle_key.h"
#include "msg.h"
#include "task_manager.h"
#include "task_common.h"
#include "audio/dac_api.h"
#include "dac.h"
#include "power_manage_api.h"
#include "wdt.h"
#include "warning_tone.h"
#include "charge.h"


#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif

#define TASK_IDLE_DEBUG_ENABLE

#ifdef TASK_IDLE_DEBUG_ENABLE
#define task_idle_printf otp_printf
#else
#define task_idle_printf(...)
#endif// TASK_IDLE_DEBUG_ENABLE


static tbool task_idle_skip_check(void **priv)
{
    task_idle_printf("task_idle_skip_check !!\n");
    return true;//
}


static void *task_idle_init(void *priv)
{
    dac_channel_on(MUSIC_CHANNEL, 0);
    warning_tone_play(TONE_POWER_OFF, 0);

    task_idle_printf("task_idle_init !!\n");
    return NULL;
}

static void task_idle_exit(void **hdl)
{
    task_idle_printf("task_idle_exit !!\n");
}

static void task_idle_deal(void *hdl)
{
    int error = MSG_NO_ERROR;
    int msg = NO_MSG;
    task_idle_printf("task_idle_deal !!\n");
    u32 i = 0;
    u8 flag = 0;

    otp_printf("task_idle_deal !!\n");

    while (1) {

        clear_wdt();
        task_get_msg(0, 1, &msg);
        if (task_common_msg_deal(hdl, msg) == false) {
            return;
        }
        if (NO_MSG == msg) {
            continue;
        }

        switch (msg) {
        case MSG_HALF_SECOND:
            task_idle_printf("-I_H-");
            break;

        case SYS_EVENT_DEC_END:
            break;

        case SYS_EVENT_PLAY_SEL_END:
            puts("SYS_EVENT_PLAY_SEL_END\n");
            task_idle_printf("idle power off\n");
            if (get_ldo5v_online_flag()) {
                JL_POWER->CON |= BIT(4);
                while (1);
            } else  {
                enter_sys_soft_poweroff();
            }
            break;

        case MSG_POWER_OFF:
            task_idle_printf("idle power off\n");
            enter_sys_soft_poweroff();
            break;

        default:
            break;
        }
    }
}

const TASK_APP task_idle_info = {
    .skip_check = task_idle_skip_check,
    .init 		= task_idle_init,
    .exit 		= task_idle_exit,
    .task 		= task_idle_deal,
    .key 		= &task_idle_key,
};


#include "msg.h"
#include "uart.h"
/* #include "audio/audio.h" */
#include "audio/dac_api.h"
#include "music_player.h"
#include "task_common.h"
#include "warning_tone.h"
#include "rec_api.h"
#include "task_rec.h"
#include "task_rec_key.h"
#include "string.h"
#include "wdt.h"
#include "fat_io.h"
#include "rec_play.h"


#define REC_TASK_DEBUG_ENABLE

#ifdef REC_TASK_DEBUG_ENABLE
#define rec_task_printf printf
#else
#define rec_task_printf(...)
#endif

#if REC_TASK_EN

#define tone_play	warning_tone_play

RECORD_OP_API *rec_mic_api = NULL;

static void *task_rec_init(void *priv)
{
    rec_task_printf("task rec init !!\n");
    fat_init();
    tone_play(TONE_REC_MODE, 0);
    return NULL;
}

static void task_rec_exit(void **hdl)
{
    task_clear_all_message();
    rec_exit(&rec_mic_api);
    recode_play_mutex_stop(NULL);
    fat_del();
    rec_task_printf("task_rec_exit !!\n");
}


static void task_rec_deal(void *p)
{
    int msg;
    int msg_error = MSG_NO_ERROR;
    tbool ret = true;

    printf("****************REC TSAK*********************\n");

    while (1) {

        clear_wdt();

        msg_error = task_get_msg(0, 1, &msg);
        if (NO_MSG == msg) {
            continue;
        }

        rec_msg_deal_api(&rec_mic_api, msg); //record 流程

        if (task_common_msg_deal(NULL, msg) == false) {
            music_tone_stop();
            task_common_msg_deal(NULL, NO_MSG);
            return;
        }

        switch (msg) {
        case SYS_EVENT_PLAY_SEL_END: //提示音结束
            rec_task_printf("RECORD_SYS_EVENT_PLAY_SEL_END\n");

        case MSG_REC_INIT:
            rec_task_printf("MSG_REC_INIT\n");
            dac_channel_on(DAC_DIGITAL_CH, FADE_ON);
            dac_set_samplerate(48000, 0);//
            break;

        case MSG_HALF_SECOND: {
            u32 tmp_rec_time = rec_get_enc_time(rec_mic_api);
            if (tmp_rec_time) {
                rec_task_printf("rec time %d:%d\n", tmp_rec_time / 60, tmp_rec_time % 60);
            }
        }
        break;

        default:
            break;
        }
    }
}

const TASK_APP task_rec_info = {
    .skip_check = NULL,
    .init 		= task_rec_init,
    .exit 		= task_rec_exit,
    .task 		= task_rec_deal,
    .key 		= &task_rec_key,
};
#endif

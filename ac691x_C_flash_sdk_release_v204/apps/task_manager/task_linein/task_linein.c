#include "task_linein.h"
#include "task_linein_key.h"
#include "msg.h"
#include "task_manager.h"
#include "task_common.h"
#include "audio/dac_api.h"
/* #include "audio/audio.h" */
#include "dac.h"
#include "power_manage_api.h"
#include "dev_manage.h"
#include "common/common.h"
#include "warning_tone.h"
#include "audio/linein_api.h"
#include "audio/ladc.h"
#include "music_player.h"
#include "led.h"


#define tone_play	warning_tone_play
#define ui_close_aux(...)
#define ui_open_aux(...)

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif

#define TASK_LINEIN_DEBUG_ENABLE

#ifdef TASK_LINEIN_DEBUG_ENABLE
#define task_linein_printf printf
#else
#define task_linein_printf(...)
#endif// TASK_LINEIN_DEBUG_ENABLE

#if LINEIN_EN
AUX_VAR g_aux_var = {0}; ///<line in检测变量

/*----------------------------------------------------------------------------*/
/**@brief  供AD_KEY 复用AUX检测时设置当前aux状态和获取aux状态*/
/*----------------------------------------------------------------------------*/
u8  get_aux_sta(void)
{
    return g_aux_var.bDevOnline;
}

void set_aux_sta(u8 sta)
{
    g_aux_var.cur_status = sta;
}

/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 在线检测实体函数
   @param  cnt：检测滤波次数
   @return 在线情况
   @note   AUX_STATUS linein_check(u8 cnt)
*/
/*----------------------------------------------------------------------------*/
AUX_STATUS linein_check(u8 cnt)
{
#if AUX_DET_MULTI_AD_KEY
    g_aux_var.cur_status = g_aux_var.cur_status;
#else
    g_aux_var.cur_status = AUX_IN_CHECK; //获取当前AUX状态
#endif
    if (g_aux_var.cur_status != g_aux_var.pre_status) {
        g_aux_var.pre_status = g_aux_var.cur_status;
        g_aux_var.status_cnt = 0;
    } else {
        g_aux_var.status_cnt++;
    }

    if (g_aux_var.status_cnt < cnt) { //消抖
        return NULL_AUX;
    }
    g_aux_var.status_cnt = 0;

    ///检测到AUX插入
    if ((AUX_OFF == g_aux_var.bDevOnline) && (!g_aux_var.pre_status)) {
        g_aux_var.bDevOnline = AUX_ON;
        return AUX_ON;
    } else if ((AUX_ON == g_aux_var.bDevOnline) && g_aux_var.pre_status) {
        g_aux_var.bDevOnline = AUX_OFF;
        return AUX_OFF;
    }

    return NULL_AUX;
}

/*----------------------------------------------------------------------------*/
/**@brief  LINE IN 在线检测调度函数
   @param  无
   @return 在线情况
   @note   s32 aux_detect(void)
*/
/*----------------------------------------------------------------------------*/
void aux_detect(void)
{
    AUX_STATUS res;

    AUX_DIR_SET;
    AUX_PU_SET;

#if AUX_DETECT_EN
    res = linein_check(20); //aux在线检测，去抖计数为50
    if (AUX_ON == res) {
        task_post_msg(NULL, 1, MSG_AUX_ONLINE);
    } else if (AUX_OFF == res) {
        task_post_msg(NULL, 1, MSG_AUX_OFFLINE);
    }
#endif
}

/* #if AUX_DETECT_EN */
/* LOOP_DETECT_REGISTER(aux_detect_loop) = { */
/* .time = 5, */
/* .fun  = aux_detect, */
/* }; */
/* #endif */

static tbool task_linein_skip_check(void **priv)
{
    task_linein_printf("task_linein_skip_check !!\n");
#if AUX_DETECT_EN
    return    g_aux_var.bDevOnline;
#else
    return    true;
#endif
}


static void *task_linein_init(void *priv)
{
    task_linein_printf("task_linein_init !!\n");
    tone_play(TONE_LINEIN_MODE, 0);
    led_fre_set(C_BLED_SLOW_MODE);

    return NULL;
}

static void linein_channel_init(void)
{
    if (LINEIN_CHANNEL == DAC_AMUX0) {
        /* PB4(L)PB5(R) */
        JL_PORTB->DIR |= BIT(4) | BIT(5);
    } else if (LINEIN_CHANNEL == DAC_AMUX1) {
        /* PA3(L)PA4(R) */
        JL_PORTA->DIR |= BIT(3) | BIT(4);
    } else if (LINEIN_CHANNEL == DAC_AMUX2) {
        //AC691X have NOT DAC_AMUX2
        /* PB6(L)PB3(R) */
        /* JL_PORTB->DIR |= BIT(6) | BIT(3); */
    }
#if AUX_AD_ENABLE
    /*
     *如果是dac其中一个通道做aux输入，由于dac通道自身阻抗
     *导致输入略小于普通aux通道。这中情况下，可以讲作为aux
     *输入的通道增益减小。比如DAC_Left作为aux输入：
     *set_sys_vol(1, sound.vol.sys_vol_r, FADE_ON);
     */
    linein_channel_open(LINEIN_CHANNEL, 0);
    ladc_ch_open(LADC_LINLR_CHANNEL, SR44100);
#else
    dac_channel_off(MUSIC_CHANNEL, FADE_ON);
    delay_2ms(20);
    dac_channel_on(LINEIN_CHANNEL, FADE_ON);
#endif
    delay_2ms(100);
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_ON);
}

static void task_linein_exit(void **hdl)
{
    task_clear_all_message();
#if AUX_AD_ENABLE
    ladc_ch_close(LADC_LINLR_CHANNEL);
#endif
    dac_channel_off(LINEIN_CHANNEL, FADE_ON);
    dac_channel_on(MUSIC_CHANNEL, FADE_ON);
    if (is_dac_mute()) {
        dac_mute(0, 0);
    }
    ui_close_aux();

    task_linein_printf("task_linein_exit !!\n");
}

static void task_linein_deal(void *hdl)
{
    int error = MSG_NO_ERROR;
    int msg = NO_MSG;
    u8 linein_start = 0;

    task_linein_printf("***********************LINEIN  TASK*********************\n");

    while (1) {

        error = task_get_msg(0, 1, &msg);
        if (task_common_msg_deal(hdl, msg) == false) {
            music_tone_stop();
            task_common_msg_deal(NULL, NO_MSG);
            return ;
        }

        if (NO_MSG == msg) {
            continue;
        }

        //task_linein_printf("linein msg = %x\n", msg);
        switch (msg) {
        case MSG_HALF_SECOND:
            //task_linein_printf("-H-");
            break;

        case SYS_EVENT_PLAY_SEL_END:
            if (linein_start == 0) {
                linein_start = 1;
            }

            linein_channel_init();
            task_linein_printf("Linein SYS_EVENT_DEC_END\n");
            break;

        case MSG_AUX_MUTE:
            task_linein_printf("MSG_AUX_MUTE\n");
            if (is_dac_mute()) {
                linein_mute(0);
                dac_mute(0, 1);
                led_fre_set(C_BLED_SLOW_MODE);
            } else {
                dac_mute(1, 1);
                linein_mute(1);
                led_fre_set(C_BLED_ON_MODE);
            }
            break;

        default:
            break;
        }
    }
}

const TASK_APP task_linein_info = {
    /* .name 		= TASK_APP_linein, */
    .skip_check = task_linein_skip_check,
    .init 		= task_linein_init,
    .exit 		= task_linein_exit,
    .task 		= task_linein_deal,
    .key 		= &task_linein_key,
};
#endif

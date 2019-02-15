#include "aec_main.h"
#include "uart.h"
#include "aec_user.h"
#include "audio/ladc_api.h"
#include "audio/ladc.h"
#include "cpu/audio_param.h"
#include "audio/dac_api.h"
#include "audio/dac.h"
#include "audio/dac_cpu.h"
#include "audio_improve_main.h"
#include "aec/sync_out.h"
#include "aec/loss_packets_repair.h"
#include "string.h"
#include "compress.h"
#include "sdk_cfg.h"
#include "clock.h"
#include "bluetooth/avctp_user.h"
#include "msg.h"
#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".aec_app_bss")
#pragma data_seg(	".aec_app_data")
#pragma const_seg(	".aec_app_const")
#pragma code_seg(	".aec_app_code")
#endif

AEC_INTERFACE aec_interface = {
    .fill_dac_echo_buf 	= NULL,
    .fill_adc_ref_buf  	= NULL,
    .aec_set_mic_gain  	= ladc_mic_gain,
    .aec_run 			= NULL,
    .kick_start 		= 0,
    .toggle				= 1,
};

/*
***********************************************************************************
*						AEC MEM MODULE
*
*Description:
*
*Argument(s):
*
*Returns	:
*
*Note(s)	:
***********************************************************************************
*/
//u8 AecBuf[26748 + 484 + 32 ] AT(.bt_aec) __attribute__((aligned(4)));	/*aec(26172) sync_in(484) reserved(32)*/
u8 AecBuf[22000 ] AT(.bt_aec) __attribute__((aligned(4)));	/*aec(26172) sync_in(484) reserved(32)*/
static u8 *p_AecBuf = NULL;
static u8 *p_SyncInBuf = NULL;
#define AEC_EN_BITMAP	(AEC_BITMAP_BT_CALLING)

void aec_buf_init()
{
    u32 aec_bufsize = 0;
    u32 sync_in_bufsize = 0;

    aec_bufsize  = aec_query_bufsize(AEC_EN_BITMAP);/*26748 bytes*/
    sync_in_bufsize = sync_in_querybuf();			/*484 bytes*/
    //otp_printf("aec_bufsize:%d\n", aec_bufsize);
    //otp_printf("sync_in_bufsize:%d\n", sync_in_bufsize);
    //otp_printf("AEC_total_buf:%d\n", aec_bufsize + sync_in_bufsize);
    p_AecBuf = (u8 *)AecBuf;
    p_SyncInBuf = (u8 *)p_AecBuf + aec_bufsize;
}

u32 aec_buf_free()
{
    p_AecBuf = NULL;
    p_SyncInBuf = NULL;
    return AEC_ERR_NONE;
}
/*
***********************************************************************************
*						AEC TASK MODULE
*
*Description:
*
*Argument(s):
*
*Returns	:
*
*Note(s)	:
***********************************************************************************
*/
static int resume_max = 0;
static void aec_task_resume()
{
    aec_interface.kick_start++;
    if (aec_interface.kick_start > resume_max) {
        resume_max = aec_interface.kick_start;
    }
}
QLZ(.qlz_sco)
static void aec_task_run(void *priv)
{
    if (aec_interface.kick_start) {
        aec_interface.kick_start--;
        aec_run(priv);
    }
}

static void aec_task_start()
{
    resume_max = 0;
    aec_handle_register(aec_task_resume);
    aec_interface.kick_start = 0;
    aec_interface.aec_run = aec_task_run;
    aec_interface.fill_adc_ref_buf = fill_adc_ref_buf;
    aec_interface.fill_dac_echo_buf = fill_dac_echo_buf;
    printf("----------%s\n", __func__);
}

static void aec_task_stop()
{
    otp_printf("resume_max:%d\n", resume_max);
    aec_interface.aec_run = NULL;
}

void aec_task_main(void)
{
    if (aec_interface.aec_run) {
        aec_interface.aec_run(NULL);
    }
}

/*
***********************************************************************************
*						AEC MODULE OPEN
*
*Description: This function is called when SCO/eSCO connect
*
*Argument(s): none
*
*Returns	: AEC_ERR_NONE	success
*			  others		faild
*
*Note(s)	:
***********************************************************************************
*/
u32 aec_start_api()
{
    u32 err = AEC_ERR_NONE;

    //puts("aec_open_api\n");

    if (p_AecBuf) {
        return AEC_ERR_EXIST;
    }

    aec_buf_init();
    if (p_AecBuf == NULL) {
        puts("aec_err0\n");
        return AEC_ERR_MEMORY;
    }

    err = aec_param_init();
    if (err != AEC_ERR_NONE) {
        puts("aec_err1\n");
        return err;
    }

    err = aec_init(p_AecBuf, &aec_param);
    if (err != AEC_ERR_NONE) {
        puts("aec_err3\n");
        return err;
    }

    err = sync_in_init(p_SyncInBuf);
    if (err != AEC_ERR_NONE) {
        puts("aec_err4\n");
        return err;
    }

    aec_task_start();

    return err;
}

/*
***********************************************************************************
*						AEC MODULE CLOSE
*
*Description: This function is called when SCO/eSCO disconnect
*
*Argument(s): none
*
*Returns	: AEC_ERR_NONE/TH_ERR_NONE	success
*			  others					failed
*
*Note(s)	:
***********************************************************************************
*/
u32 aec_stop_api()
{
    //puts("aec_stop_api\n");
    aec_close();
    aec_task_stop();
    aec_buf_free();
    return 0;
}

/*
***********************************************************************************
*						PHONE_AUDIO_INIT
*
*Description: This function is called when SCO/eSCO change state
*
*Argument(s): en = 1,phone audio enable
*			  en = 0,phone audio disable
*
*Returns	: none
*
*Note(s)	: 1)
***********************************************************************************
*/
static u8 tmp_sys_vol = 0xff;
void phone_audio_init(u8 en)
{
    u8 default_phone_vol = 15;
    if (en) {
        set_apc_clk(CALL_APC_Hz);
        ladc_reg_init(SR8000);/*open mic_adc*/
        ladc_mic_gain(aec_param.mic_analog_gain, 0);/*set mic gain*/
        dac_set_samplerate(SR8000, 1);/*set dac samplerate*/
        tmp_sys_vol = get_sys_vol_api(1);/*save current system volume*/
        /*limit phone_vol max*/
        sys_max_vol_init(aec_param.dac_analog_gain, aec_param.dac_analog_gain, MAX_DIGITAL_VOL_L, MAX_DIGITAL_VOL_R);
        /*set current phone_vol*/
        dac_ctl.sys_vol_l = aec_param.dac_analog_gain;
        dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
        set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
        /* user_send_cmd_prepare(USER_CTRL_HFP_CALL_SET_VOLUME, 1, &default_phone_vol); //设置电话通话的初始音量值 */
    } else {
        ladc_close(); /*close mic_adc*/
        set_apc_clk(NORMAL_APC_Hz);
        sys_max_vol_init(dac_ctl.sys_vol_max, dac_ctl.sys_vol_max, MAX_DIGITAL_VOL_L, MAX_DIGITAL_VOL_R);
        dac_ctl.sys_vol_l = tmp_sys_vol;
        dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
        set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
    }
}

/*
***********************************************************************************
*						SCO	CALLBACK
*
*Description: This function is called when SCO/eSCO connect or disconnect
*
*Argument(s):
*
*Returns	:
*
*Note(s)	:
***********************************************************************************
*/
static u16 packets_dump;
QLZ(.qlz_sco)
s32 aec_sco_conn(void *priv)
{
#if BT_HFP_EN_SCO_DIS
    user_send_cmd_prepare(USER_CTRL_DISCONN_SCO, 0, NULL);
    return 0;
#endif
    dac_cbuf_clear();
    audio_repair_init();
    audio_sync_out_init();
    aec_start_api();
    phone_audio_init(1);
    aec_buf_clear();
    packets_dump = 0;
#if (CALL_USE_DIFF_OUTPUT && VCOMO_EN)
    VCM_OUT_EN(0);
#endif
    return 0;

}

QLZ(.qlz_sco)
s32 aec_sco_disconn(void *priv)
{
#if BT_HFP_EN_SCO_DIS
    return 0;
#endif
    aec_stop_api();
    sync_out_exit();
    audio_repair_exit();
    phone_audio_init(0);
    packets_dump = 0;
    dac_cbuf_clear();
#if (CALL_USE_DIFF_OUTPUT && VCOMO_EN)
    VCM_OUT_EN(1);
#endif
    return 0;
}

s32 hook_sco_conn(void *priv)
{
    puts("sco_conn_cb\n");
    if (aec_param.aec_ctl == AEC_REDUCE) {
        set_sys_freq(BT_CALL_REDUCE_Hz);
    } else {
        set_sys_freq(BT_CALL_Hz);
    }
    dac_automute_set(0, -1, -1, -1); // 关自动mute
    /* put_event(EVENT_AUTOMUTE_OFF); */
    return aec_sco_conn(priv);
}

s32 hook_sco_disconn(void *priv)
{
    puts("sco_disconn_cb\n");
    set_sys_freq(SYS_Hz);
    /* put_event(EVENT_AUTOMUTE_ON); */
    return aec_sco_disconn(priv);
}

void sco_2_dac(s16 *data, int point);
QLZ(.qlz_sco)
void aec_sco_rx(s16 *data, u16 point, u8 sco_flags)
{
    if (packets_dump < 60) {
        packets_dump++;
        memset(data, 0, point << 1);
        sco_flags = 0;
    }

    audio_repair_run(data, data, point, sco_flags);

    if (AEC_EN_BITMAP & AEC_BITMAP_SPEECH_DET) {
        Speech_Dectection_Api(data, point);
    } else {
        sync_out_run((u8 *)data, point << 1);
        //sco_2_dac(data,point);
    }
}


extern volatile u8 esco_coder_busy_flag ;
void hook_sco_rx(u8 *data, u16 point, u8 sco_flags)
{
    if (esco_coder_busy_flag == 0x55) {
        aec_sco_rx((s16 *)data, point, sco_flags);
    }
}

void sco_2_dac(s16 *buf, int point)
{
    s16 *data = (s16 *)buf;
    s32 temp, i;
    s16 temp_buf[64];

    while (point) {
        temp = (point > 32) ? 32 : point;
        point = point - temp;
        for (i = 0; i < temp; i++) {
            temp_buf[2 * i] = *data;
            temp_buf[2 * i + 1] = *data++;
        }
        dac_write(temp_buf, temp * 4);
    }
}



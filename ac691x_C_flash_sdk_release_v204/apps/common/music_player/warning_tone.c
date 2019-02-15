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
#include "sys_cfg.h"

#include "audio/dac_api.h"
#include "audio/dac.h"

#include "dec/if_decoder_ctrl.h"

#include "mask_tone.h"
#include "dec/warning_tone.h"
#include "music_player.h"
#include "res_file.h"



#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".system_bss")
#pragma data_seg(	".system_data")
#pragma const_seg(	".system_const")
#pragma code_seg(	".system_code")
#endif

extern decoder_ops_t *get_wt_ops(void);

decoder_ops_t nwt_ops;			//重写get_wt_ops
WARNING_PARM curr_tone_parm;	//记录当前播放的tone_parm
struct if_decoder_io *decoder_io_p;
TONE_VAR_T tone_var;

typedef struct __SBC_NOTICE_CTRL {
    MUSIC_DECODER *dec;
    RES_DIR_T info;
    u32 read_ptr;
    u8  index;
    u8  repeat_flag;
} SBC_NOTICE_CTRL;

static SBC_NOTICE_CTRL sbc_notice_hdl;


#define MS_2_BYTE		(48*2*2)

static void slot_delay(u32 time)
{
    u32 write_cnt, remain_cnt;
    u8 silent[MS_2_BYTE];
    memset(silent, 0x00, sizeof(silent));
    while (time--) {
        /* puts("*"); */
        remain_cnt = sizeof(silent);
        while (remain_cnt) {
            write_cnt = decoder_io_p->output(decoder_io_p->priv, silent, remain_cnt);
            remain_cnt -= write_cnt;
        }
    }
}

static u32 nwt_run(void *work_buf, u32 type)
{
    u32 run_ret;

    decoder_ops_t *ops_hd = get_wt_ops();

    run_ret = ops_hd->run(work_buf, type);

    if (0 != run_ret) { //deocde_end
        if (curr_tone_parm.cur_cnt < curr_tone_parm.set_cnt - 1) {
            curr_tone_parm.cur_cnt++;
            slot_delay(curr_tone_parm.slot_time);
            run_ret = ops_hd->open(work_buf, decoder_io_p, (void *)&curr_tone_parm.set_p[curr_tone_parm.cur_cnt]);
            return 0;
        } else {
            return run_ret;
        }
    } else {
        return 0;
    }
}

static u32 nwt_open(void *work_buf, const struct if_decoder_io *decoder_io, u8 *bk_point_ptr)
{
    WARNING_PARM *tones_parm_p = (void *)bk_point_ptr;
    decoder_ops_t *ops_hd = get_wt_ops();

    memcpy(&curr_tone_parm, bk_point_ptr, sizeof(curr_tone_parm));

    decoder_io_p = (struct if_decoder_io *)decoder_io;
    curr_tone_parm.cur_cnt = 0;

    return ops_hd->open(work_buf, decoder_io, (void *)&curr_tone_parm.set_p[curr_tone_parm.cur_cnt]);
}

decoder_ops_t *get_nwt_ops(void)
{
    memcpy(&nwt_ops, get_wt_ops(), sizeof(nwt_ops));

    //rewrite run and open function
    nwt_ops.run = nwt_run;
    nwt_ops.open = nwt_open;

    return &nwt_ops;
}
#include "audio/src.h"
typedef struct _TONE_SRC {
    volatile u8 busy;
    volatile u8 toggle;
} TONE_SRC;
TONE_SRC tone_src;
void tone_src_output(u8 *buf, u16 len, u8 flag)
{
    u32 wlen = 0;

    if (flag & BIT(0)) {
        wlen = dac_write((s16 *)buf, (u32)len);
        if (wlen != len) {
            otp_printf("[%d--%d]", wlen, len);
        }
    }

    if (flag == BIT(0)) {
        /*src indat remaind,contine*/
        tone_src.busy = 1;
        src_write_api(NULL, SRC_IDAT_LEN_MAX);
    } else {
        tone_src.busy = 0;
    }
}

s32 tone_src_init(void)
{
    s32 err = 0;
    u32 size;
    src_param_t src_p;

    src_p.in_chinc = 1;
    src_p.in_spinc = 2;
    src_p.out_chinc = 1;
    src_p.out_spinc = 2;
    src_p.in_rate = 8000;
    src_p.out_rate = 9000;
    src_p.nchannel = 2;
    src_p.output_cbk = (void *)tone_src_output;

    size = src_mem_query(SRC_IDAT_LEN_MAX, SRC_ODAT_LEN_MAX, SRC_FLTB_MAX);
    err = src_init_api(&src_p, src_buffer);
    tone_src.toggle = 1;
    tone_src.busy = 0;
    return err;
}

void tone_src_exit(void)
{
    src_exit_api();
    tone_src.toggle = 0;
}

u32 tone_src_run(void *buf, u32 len)
{
    u32 wlen = 0;

    if (tone_src.toggle) {
        while (tone_src.busy) {
            putchar('w');
        };
        tone_src.busy = 1;
        if (len > SRC_IDAT_LEN_MAX) {
            wlen = SRC_IDAT_LEN_MAX;
        } else {
            wlen = len;
        }
        src_write_api(buf, wlen);
    } else {
        wlen = dac_write((s16 *)buf, (u32)len);
    }
    return wlen;
}

static u32 music_set_info(void *priv, dec_inf_t *inf, tbool wait)
{
    if (sbc_notice_hdl.info.type) {
        //sbc file is 8k sample rate
        if (inf->sr == 48000) {
            inf->sr = 8000;
        }
        /* printf("<<<<<sr:%d\n",inf->sr); */
        dac_set_samplerate(inf->sr, wait);
    } else {
        //nwt
        dac_set_samplerate(inf->sr, wait);
    }
    return 0;
}

extern const u16 digital_vol_tab[MAX_DIGITAL_VOL_L + 1];
void tone_digit_vol(void *buf, u32 len, u8 vol)
{
    u16 i;
    s16 *dat = buf;
    s32 tmp;

    for (i = 0; i < (len >> 1); i++) {
        tmp = dat[i];
        tmp = (tmp * digital_vol_tab[vol]) >> 14;
        dat[i] = (s16)tmp;
    }
}
static int music_output(void *priv, void *buf, u32 len)
{
    int out_len = is_dac_write_able(0);
    if (out_len) {
        if (out_len > len) {
            out_len = len;
        }

        tone_digit_vol(buf, out_len, 25);/*vol = 0~MAX_DIGITAL_VOL_L*/

        dac_write((s16 *)buf, out_len);
        music_decoder_loop_resume();   /* _thread_reset_resume_byname(MUSIC_PLAYER_TASK, 1); */
        return out_len;
    } else {
        return 0;
    }
}


static void sbc_notice_decoder_err_deal(void *priv, u32 err)
{
    u32 msg;
    if (err && !sbc_notice_hdl.repeat_flag) {
        /* otp_printf("sbc notice dec err = %x\n", err); */
        mutex_resource_release("tone");
    }
}



static u32 sbc_notice_file_read(void *priv, u8 *buf, u16 len)
{
    SBC_NOTICE_CTRL *hdl = (SBC_NOTICE_CTRL *)priv;
    if (hdl == NULL) {
        return (u32) - 1;
    }

    if (hdl->read_ptr >= hdl->info.len) {
        return (u32) 0;
    }

    memcpy(buf, (u8 *)(hdl->info.addr + hdl->read_ptr), len);
    hdl->read_ptr += len;

    /* otp_printf_buf(buf, len); */

    return len;
}

static s32 sbc_notice_file_get_size(void *priv, u32 *size)
{
    SBC_NOTICE_CTRL *hdl = (SBC_NOTICE_CTRL *)priv;
    if (hdl == NULL) {
        return 0;
    }

    return hdl->info.len;
}

const DEC_FILE_IO sbc_notice_file_io = {
    .seek = NULL,
    .read = sbc_notice_file_read,
    .get_size = sbc_notice_file_get_size,
};

enum {
    TONE_POWER_ON_NUM = 0,
    TONE_POWER_OFF_NUM	,
    TONE_PAIRING_NUM	,
    TONE_WARNING_NUM	,
    TONE_CONN_NUM		,
    TONE_DISCONN_NUM	,
    TONE_RING_NUM		,
};

tbool get_otp_tone_info(u8 idx, RES_DIR_T *ptr)
{
    __RES_INFO_T *otp_tone_info = (__RES_INFO_T *)cfg_info_addr[CFG_ADR_TONE_INFO];
    if ((ptr == NULL) || (otp_tone_info == NULL)) {
        puts("otp_tone_err\n");
        return false;
    }
#if 0
    put_buf((void *)otp_tone_info, 512);
    bt_tone_printf("total:0x%x\n", otp_tone_info->total);
    u8 i = 0;
    for (i = 0; i < otp_tone_info->total; i++) {
        bt_tone_printf("addr:0x%x\tlen:0x%x\ttype:0x%x\n", otp_tone_info->data[i].addr, otp_tone_info->data[i].len, otp_tone_info->data[i].type);
        put_buf((void *)((u32)otp_tone_info->data[i].addr + (u32)otp_tone_info), otp_tone_info->data[i].len);
    }
#endif

    if (otp_tone_info->total && (idx < otp_tone_info->total)) {
        ptr->addr = (u32)otp_tone_info->data[idx].addr + (u32)otp_tone_info;
        ptr->len = (u32)otp_tone_info->data[idx].len;
        ptr->type = (u32)otp_tone_info->data[idx].type;
        return true;		//find file succ
    }
    return false;			//find file fail
}

static void notice_play(void *priv)
{
    /* bt_tone_printf("---------------play--------------------------\n"); */

    u8 tone_index = (u8)priv;
    MUSIC_DECODER *obj = NULL;

    if (get_otp_tone_info(sbc_notice_hdl.index, &(sbc_notice_hdl.info)) == false) {		//fail and return
        return;
    }

    sbc_notice_hdl.read_ptr = 0;

    obj = music_decoder_creat();
    if (obj == NULL) {
        printf("music player creat fail ！\n");
        return ;
    }

    dac_automute_set(0, -1, -1, -1); // 关自动mute
    dac_mute(1, 1);
    dac_cbuf_clear();
    dac_buf_clear();
    dac_mute(0, 1);
    /* set_sys_vol(SYS_DEFAULT_VOL, SYS_DEFAULT_VOL, FADE_OFF);//固定音量播提示音 */
    sbc_notice_hdl.dec = obj;

    music_decoder_set_file_interface(obj, (DEC_FILE_IO *)&sbc_notice_file_io, &sbc_notice_hdl/*使用者主注册的file_io的私有属性*/);
    music_decoder_set_data_clear_interface(obj, NULL, NULL);
    music_decoder_set_data_wait_interface(obj, NULL, NULL);
    music_decoder_set_setInfo_interface(obj, music_set_info, NULL);
    music_decoder_set_output_interface(obj, music_output, NULL);
    music_decoder_set_err_deal_interface(obj, sbc_notice_decoder_err_deal, NULL);

    if (sbc_notice_hdl.info.type) {
        puts("sbc tone\n");
        music_decoder_set_spec_format_name(obj, "SBC");
        music_decoder_set_decoder_type(obj, DEC_PHY_SBC);
    } else {
        puts("nwt tone\n");
        music_decoder_set_break_point(obj, (void *)sbc_notice_hdl.info.addr, 0);
        music_decoder_set_spec_format_name(obj, "WT");
        music_decoder_set_decoder_type(obj, DEC_PHY_NWT);
    }

    tbool err = music_decoder_play(obj);
    if (err == MUSIC_DECODER_ERR_NONE) {
        /* bt_tone_printf("music_player_play ok !! fun = %s, line = %d\n", __func__, __LINE__); */
    } else {
        printf("music_player_play dec fail %x !! fun = %s, line = %d\n", err, __func__, __LINE__);
    }
}

static void notice_stop(void *hdl)
{
    /* bt_tone_printf("---------------stop--------------------------\n"); */
    music_decoder_destroy((MUSIC_DECODER **) & (sbc_notice_hdl.dec));
    //dac_cbuf_clear();
    dac_automute_set(AUTO_MUTE_CFG, -1, -1, -1); // 开自动mute
}


#if 0
static void sbc_notice_decoder_err_deal(void *priv, u32 err)
{
    u32 msg;
    if (err && !sbc_notice_hdl.repeat_flag) {
        /* log_printf("sbc notice dec err = %x\n", err); */
        mutex_resource_release("tone");
    }
}



static u32 sbc_notice_file_read(void *priv, u8 *buf, u16 len)
{
    SBC_NOTICE_CTRL *hdl = (SBC_NOTICE_CTRL *)priv;
    if (hdl == NULL) {
        return (u32) - 1;
    }

    if (hdl->read_ptr >= hdl->info.len) {
        return (u32) 0;
    }

    memcpy(buf, (u8 *)(hdl->info.addr + hdl->read_ptr), len);
    hdl->read_ptr += len;

    /* log_printf_buf(buf, len); */

    return len;
}

static u32 sbc_notice_file_get_size(void *priv)
{
    SBC_NOTICE_CTRL *hdl = (SBC_NOTICE_CTRL *)priv;
    if (hdl == NULL) {
        return 0;
    }

    return hdl->info.len;
}

const AUDIO_FILE sbc_notice_file_io = {
    .seek = NULL,
    .read = sbc_notice_file_read,
    .get_size = sbc_notice_file_get_size,
};

static void notice_play(void *priv)
{

    u8 tone_index = (u8)priv;
    MUSIC_DECODER *obj = NULL;

    sbc_notice_hdl.index = tone_var.idx;
    sbc_notice_hdl.repeat_flag = tone_var.rpt_mode;
    sbc_notice_hdl.read_ptr = 0;

    tone_var.status = 1;
    obj = music_decoder_creat();
    if (obj == NULL) {
        return ;
    }

    sound_automute_set(0, -1, -1, -1); // 关自动mute
    dac_mute(0, 1);
    /* set_sys_vol(SYS_DEFAULT_VOL, SYS_DEFAULT_VOL, FADE_ON);//固定音量播提示音 */
    sbc_notice_hdl.dec = obj;

    AUDIO_STREAM_PARAM stream_param;
    stream_param.ef = AUDIO_EFFECT_NULL;
    stream_param.ch = 2;
    stream_param.sr = SR44100;
    music_decoder_set_output(obj, audio_stream_init(&stream_param, NULL));
    music_decoder_set_file_interface(obj, (AUDIO_FILE *)&sbc_notice_file_io, &sbc_notice_hdl/*使用者主注册的file_io的私有属性*/);
    music_decoder_set_err_deal_interface(obj, sbc_notice_decoder_err_deal, NULL);

    music_decoder_set_configs(obj, (DEC_CFG *)notice_nwt_decoder_configs, 1);

    tbool err = music_decoder_play(obj, NULL);
    if (err == MUSIC_DECODER_ERR_NONE) {
    } else {
    }
}

static void notice_stop(void *hdl)
{
    puts("notice_stop\n");
    music_decoder_destroy((MUSIC_DECODER **) & (sbc_notice_hdl.dec));
    //dac_cbuf_clear();
    sound_automute_set(AUTO_MUTE_CFG, -1, -1, -1); // 开自动mute
    tone_var.status = 0;
}


#endif

u8 get_tone_status(void)
{
    return tone_var.status;
}

void *get_number_tone(u32 num)
{
    switch (num) {
    case 0:
        return RES_0_MP3;
    case 1:
        return RES_1_MP3;
    case 2:
        return RES_2_MP3;
    case 3:
        return RES_3_MP3;
    case 4:
        return RES_4_MP3;
    case 5:
        return RES_5_MP3;
    case 6:
        return RES_6_MP3;
    case 7:
        return RES_7_MP3;
    case 8:
        return RES_8_MP3;
    case 9:
        return RES_9_MP3;
    }
    return NULL;
}

static void mp3_tone_mutex_play(void *priv)
{
    void *tone_name = NULL;
    //printf("idx:%d\n", tone_var.idx);
    switch (tone_var.idx) {
    case TONE_POWER_ON:
    case TONE_BT_MODE:
        tone_name = RES_BT_MP3;
        break;
    case TONE_POWER_OFF:
        tone_name = RES_POWER_OFF_MP3;
        break;
    case TONE_BT_CONN:
        tone_name = RES_CONNECT_MP3;
        break;
    case TONE_BT_DISCON:
        tone_name = RES_DISCONNECT_MP3;
        break;
    case TONE_BT_PARING:
        break;
    case TONE_RING:
        tone_name = RES_RING_MP3;
        break;
    case TONE_WARNING:
        tone_name = RES_WARNING_MP3;
        break;
    case TONE_MUSIC_MODE:
        tone_name = RES_MUSIC_MP3;
        break;
    case TONE_RADIO_MODE:
        tone_name = RES_RADIO_MP3;
        break;
    case TONE_LINEIN_MODE:
        tone_name = RES_LINEIN_MP3;
        break;
    case TONE_REC_MODE:
        tone_name = RES_REC_MP3;
        break;
    case TONE_ECHO_MODE:
        tone_name = RES_ECHO_MP3;
        break;
    case TONE_PC_MODE:
        tone_name = RES_PC_MP3;
        break;
    case TONE_RTC_MODE:
        tone_name = RES_RTC_MP3;
        break;
    case TONE_NUM_0:
    case TONE_NUM_1:
    case TONE_NUM_2:
    case TONE_NUM_3:
    case TONE_NUM_4:
    case TONE_NUM_5:
    case TONE_NUM_6:
    case TONE_NUM_7:
    case TONE_NUM_8:
    case TONE_NUM_9:
        tone_name = get_number_tone(tone_var.idx - TONE_NUM_0);
        break;
    }


    if (tone_name) {
        music_tone_play(tone_name);
    } else {
        puts("tone NULL,release mutex\n");
        mutex_resource_release("tone");
    }
}

static void mp3_tone_mutex_end(void *priv)
{
    puts("mp3_tone_end\n");
    music_tone_end();
    delay_2ms(45);
}

void warning_tone_play(u8 index, u8 repeat_flag)
{
    u32 res;
#if (BT_MODE != NORMAL_MODE)
    return;
#endif
    if (get_going_to_pwr_off() && (index != TONE_POWER_OFF)) { //关机过程不播其它提示音
        return;
    }
    printf("tone_index:%d\n", index);
    tone_var.idx = index;
    tone_var.rpt_mode = repeat_flag;
    if (index < TONE_NWT_MAX) {
        res = mutex_resource_apply("tone", 5, notice_play, notice_stop, NULL);
    } else {
        res = mutex_resource_apply("tone", 5, mp3_tone_mutex_play, mp3_tone_mutex_end, NULL);
    }
    if (res == FALSE) {
        //puts("tone_play mutex apply error\n");
    }
}

#if 0
#define     DEAL_LEN         2048
static int mp_output(void *priv, void *data, int len)
{
    int len_deal = len;
    if (len_deal > DEAL_LEN) {
        while (1) {
            puts("e");
        }
        len_deal = DEAL_LEN;
    }

    while (1) {
        if (is_dac_write_able(len_deal)) {
            break;
        }
    }

    dac_write(data, len_deal);
    /* otp_printf_buf(data,len_deal); */
    return len_deal;
}

void test_for_new_tone(void)
{
    u8 i;

    extern u32 sbc_decode_ram[3 * 1024];
    void *wt = sbc_decode_ram;


    decoder_ops_t *wt_ops_p = get_nwt_ops();
    /* decoder_ops_t *wt_ops_p = get_wt_ops(); */


    struct if_decoder_io dec_io;
    memset(&dec_io, 0x00, sizeof(dec_io));
    dec_io.output = (void *)mp_output;

    /* test_sine_32k(); */

    dac_set_samplerate(SR48000, 1); /*set dac samplerate*/

    while (1) {
        for (i = 0; i < 7; i++) { //tst//
            int ret;

            wt_ops_p->open(wt, (void *)&dec_io, (void *)tones[i]);

            dec_inf_t *p = wt_ops_p->get_dec_inf(wt);

            otp_printf("ops->sr = %d\n", p->sr);

            while (1) {
                //ret = build_whole_tone(wt);
                ret = wt_ops_p->run(wt, PLAY_MOD_NORMAL);

                if (ret) {
                    delay_2ms(250);
                    break;
                }
            }
        }
    }
}
#endif


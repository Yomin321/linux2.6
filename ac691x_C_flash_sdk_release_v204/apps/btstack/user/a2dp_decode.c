#include "a2dp_decode.h"
#include "music_decoder.h"
#include "msg.h"
#include "string.h"
#include "uart.h"
#include "audio/dac_api.h"
#include "a2dp_sync.h"
#include "compress.h"
#include "audio/dac_api.h"
#include "audio/dac.h"

#define MUSIC_DECODER_API_DEBUG_ENABLE
#ifdef MUSIC_DECODER_API_DEBUG_ENABLE
#define mapi_printf otp_printf
#else
#define mapi_printf(...)
#endif//MUSIC_DECODER_API_DEBUG_ENABLE



#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".a2dp_decode_bss")
#pragma data_seg(	".a2dp_decode_data")
#pragma const_seg(	".a2dp_decode_const")
#pragma code_seg(	".a2dp_decode_code")
#endif


static volatile u16 a2dp_dec_sr = 44100;
QLZ(.qlz_a2dp)
static u32 a2dp_media_set_info(void *priv, dec_inf_t *inf, tbool wait)
{
    //otp_printf("[%d]",inf->sr);
#if A2DP_SYNC_EN
    if (inf->sr) {
        /* if(a2dp_dec_sr != 44100) {
        	otp_printf("[sr:%d]",a2dp_dec_sr);
        } */
        a2dp_dec_sr = inf->sr;
    }
    dac_set_samplerate(44100, wait);
#else
    dac_set_samplerate(inf->sr, wait);
#endif
    return 0;
}

QLZ(.qlz_a2dp)
static int a2dp_media_output(void *priv, void *buf, u32 len)
{
#if A2DP_SYNC_EN
    int out_len = (int)a2dp_sync_run(buf, len, a2dp_dec_sr);
    if (out_len != ((int)len)) {
        /* putchar('1');  */
    } else {
        /* music_decoder_loop_resume(); */
        /* putchar('2');  */
    }
#else
    int out_len = (int)dac_write((s16 *)buf, len);
    if (out_len != ((int)len)) {
        /* putchar('1');  */
    } else {
        music_decoder_loop_resume();
        /* putchar('2');  */
    }
#endif

    return out_len;
}

/*使用者自行实现， 主要是作为解码器的输入*/
QLZ(.qlz_a2dp)
DEC_FILE_IO a2dp_media_file_io = {
    .seek = NULL,
    .read = NULL,
    .get_size = NULL,
};
enum {
    A2DP_MEDIA_DEC_STOP,
    A2DP_MEDIA_DEC_WAITING,
    A2DP_MEDIA_DEC_START,
};
struct a2dp_media_file {
    cbuffer_t cbuf;
    void **decode_hdl;
    volatile int state;
    u32 media_total_len;
    u32 media_buf_ptr[0];
};
static u16 otp_a2dp_media_file_read(void *priv, u8 *buf, u16 len)
{
    u16 read_len;
    struct a2dp_media_file *file = (struct a2dp_media_file *)priv;
    int data_len;

    if (file->state != A2DP_MEDIA_DEC_START) {
        return 0;
    }

    while (file->state == A2DP_MEDIA_DEC_START) {
        data_len = cbuf_get_data_size(&file->cbuf);
        if (data_len <= len) {
            // putchar('N');
            len = data_len;
        }
        read_len = cbuf_read(&file->cbuf, buf, len);
        return read_len;
    }
    return len;
}

QLZ(.qlz_a2dp)
void *a2dp_media_start(void *priv, const char *format,
                       u16(*read_handle)(void *priv, u8 *buf, u16 len),
                       bool (*seek_handle)(void *priv, u8 type, u32 offsiz))
{
    int err;
    MUSIC_DECODER *obj = NULL;

    printf("%s \n", __func__);
    obj = music_decoder_creat();
    if (obj == NULL) {
        mapi_printf("music player creat fail ！\n");
        return NULL;
    }
    a2dp_media_file_io.read = (void *)otp_a2dp_media_file_read ;
    a2dp_media_file_io.seek = (void *)seek_handle;
    music_decoder_set_file_interface(obj, (DEC_FILE_IO *)&a2dp_media_file_io, priv/*使用者主注册的file_io的私有属性*/);
    music_decoder_set_data_clear_interface(obj, NULL, NULL);
    music_decoder_set_data_wait_interface(obj, NULL, NULL);
    music_decoder_set_setInfo_interface(obj, a2dp_media_set_info, NULL);
    music_decoder_set_output_interface(obj, a2dp_media_output, NULL);

    music_decoder_set_err_deal_interface(obj, NULL, NULL);

    music_decoder_set_spec_format_name(obj, format);

    if (0 == strcmp(format, "SBC")) {
        mapi_printf("is sbc, fun = %s, line = %d\n",  __func__, __LINE__);
        music_decoder_set_decoder_type(obj, DEC_PHY_SBC);
    }
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
#if A2DP_SYNC_EN
    err = a2dp_sync_init();
#endif
    err = music_decoder_play(obj);
    if (err != MUSIC_DECODER_ERR_NONE) {
        music_decoder_destroy(&obj);
        mapi_printf("music_player_play dec fail %x !! fun = %s, line = %d\n", err, __func__, __LINE__);
        return NULL;
    }
    return (void *)obj;
}
void *a2dp_media_play(void *priv, const char *format,
                      u16(*read_handle)(void *priv, u8 *buf, u16 len),
                      bool (*seek_handle)(void *priv, u8 type, u32 offsiz))
{
    return a2dp_media_start(priv, format, read_handle, seek_handle);
}

void a2dp_media_stop(void *hdl)
{
    printf("%s \n", __func__);
    music_decoder_destroy((MUSIC_DECODER **)&hdl);
#if A2DP_SYNC_EN
    a2dp_sync_exit();
#endif
}



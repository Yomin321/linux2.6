#include "audio_improve_main.h"
#include "aec/sync_out.h"
#include "aec/loss_packets_repair.h"
#include "audio/dac_api.h"
#include "uart.h"
#include "sdk_cfg.h"
#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".aec_app_bss")
#pragma data_seg(	".aec_app_data")
#pragma const_seg(	".aec_app_const")
#pragma code_seg(	".aec_app_code")
#endif
/*
***********************************************************************************
*					Audio Repair Module
*
*Description: This module is defined for repair loss packets
*
*Argument(s):
*
*Returns	:
*
*Note(s)	:
***********************************************************************************
*/
u32 repair_buf[336] AT(.bt_aec);
u32 repair_cnt = 0;
void audio_repair_init()
{
    s8 err = 0;
    u32 buf_size;

    repair_cnt = 0;
    buf_size = LossPacketRepair_QueryBuffSize();	/*size = 0x540(1344)*/
    //otp_printf("repair_buf:0x%x\n", buf_size);

    err = LossPacketsRepair_init(repair_buf);
    if (err != 0) {
        puts("LossPacketRepair init err\n");
    }
    //puts("LossPacketRepair init OK\n");
}

#define REPAIR_LEN_MAX	30
void audio_repair_run(s16 *inbuf, s16 *output, u16 point, u8 repair_flag)
{
    u16 repair_len, tmp_len;
    s16 *p_in, *p_out;

    p_in    = inbuf;
    p_out   = output;
    tmp_len = point;

#if 1
    if (repair_flag) {
        repair_cnt++;
        otp_printf("[E%d]", repair_cnt);
    } else {
        repair_cnt = 0;
    }
#endif

    while (tmp_len) {
        repair_len = (tmp_len > REPAIR_LEN_MAX) ? REPAIR_LEN_MAX : tmp_len;
        tmp_len = tmp_len - repair_len;
        LossPacketsRepair_run(p_in, p_out, repair_len, repair_flag);
        p_in  += repair_len;
        p_out += repair_len;
    }
}

void audio_repair_exit()
{
    LossPacketsRepair_exit();
    //puts("audio_repair_exit OK\n");
}

/*
***********************************************************************************
*					Audio Sync Output Module
*
*Description: This module is defined for sync_out
*
*Argument(s):
*
*Returns	:
*
*Note(s)	:
***********************************************************************************
*/
u32 sync_out_buf[121] AT(.bt_aec);
int get_dac_cbuf_data_len(void *arg)
{
    return get_dac_cbuf_len();
}
void audio_sync_out_init()
{
    s32 err = 0;
    u32 buf_size;
    buf_size = sync_out_querybuf();	/*size = 0x1e4(484=121*4)*/
    //otp_printf("sync_out_buf:0x%x\n", buf_size);
    sync_out_handle_register(dac_write, get_dac_cbuf_data_len);
    err = sync_out_init(sync_out_buf, 1);
    if (err != 0) {
        puts("sync_out_init err\n");
    }
    //puts("sync_out_init OK\n");
}

void audio_sync_out_run(u8 *inbuf, u32 len)
{
    sync_out_run(inbuf, len);
}

void audio_sync_out_exit()
{
    sync_out_exit();
    //puts("audio_sync_out_exit\n");
}

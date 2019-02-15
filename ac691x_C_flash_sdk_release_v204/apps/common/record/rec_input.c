#include "sdk_cfg.h"
#include "common/includes.h"
#include "common/common.h"
#include "ladc.h"
#include "rec_api.h"
#include "audio_param.h"
#include "clock.h"
#include "clock_interface.h"

#define ladc_ch_open(a,b)	ladc_reg_init(b)
#define ladc_ch_close(a)	ladc_close()


typedef struct rec_input_hdl {
    u32 sr;
    u8 channel;
    ENC_INPUT_IO  *input_io;
} REC_INPUT_HDL;
static REC_INPUT_HDL *input_hdl_p = NULL;

void *rec_input_init(void *enc_input_io, u8 ch, u32 sr)
{
    REC_INPUT_HDL *input_hdl;
    set_apc_clk(CALL_APC_Hz);
    if (ch == REC_MIC_CHANNEL) {
        ladc_ch_open(LADC_MIC_CHANNEL, sr);
        ladc_mic_gain(35, 0);
        ladc_mic_mute(0);
    }
    input_hdl = rec_malloc(sizeof(REC_INPUT_HDL));
    if (input_hdl == NULL) {
        return NULL;
    }

    input_hdl->sr = sr;
    input_hdl->channel = ch;
    input_hdl->input_io = enc_input_io;
    input_hdl_p = input_hdl;
    return (void *)input_hdl;
}

void rec_input_exit(void **input_hd_p)
{
    REC_INPUT_HDL *input_hdl;
    set_apc_clk(NORMAL_APC_Hz);
    if (input_hd_p) {
        input_hdl = *input_hd_p;
        if (input_hdl) {
            if (input_hdl->channel == REC_MIC_CHANNEL) {
                ladc_ch_close(input_hdl->channel);
            }
            rec_free_fun((void **)input_hd_p);
        }
    }

    if (input_hdl_p) {
        input_hdl_p = NULL;
    }
}

void rec_input(void *rec_input_hdl, void *buff, u32 buffer_len)
{
    REC_INPUT_HDL *hdl = (REC_INPUT_HDL *)rec_input_hdl;
    if (hdl) {
        void (*input_fun)(void *, void *, u32) = hdl->input_io->input;
        void *priv = hdl->input_io->priv;
        if (input_fun) {
            input_fun(priv, buff, buffer_len);
        }
    }
}


void rec_ladc_data_cb(s16 *buff, u32 buffer_len)
{
    if (input_hdl_p) {
        rec_input(input_hdl_p, buff, buffer_len);
    }
}


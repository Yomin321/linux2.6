#include "audio/src.h"
#include "audio/dac_api.h"
#include "cbuf/circular_buf.h"
#include "uart.h"
#include "sdk_cfg.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".dac_app_bss")
#pragma data_seg(	".dac_app_data")
#pragma const_seg(	".dac_app_const")
#pragma code_seg(	".dac_app_code")
#endif

u8 src_buffer[SRC_IDAT_LEN_MAX + SRC_ODAT_LEN_MAX + SRC_FLTB_MAX * 48] AT(.dac_buf_sec) __attribute__((aligned(4)));

#define SRC_MODULE_EN	0
#if SRC_MODULE_EN

u8 src_cbuffer[SRC_CBUFF_SIZE] AT(.dac_buf_sec) __attribute__((aligned(4)));

typedef struct __SRC_T_ {
    cbuffer_t src_cbuf;
    volatile u8 toggle;
    volatile u8 empty;
} SRC_T;
SRC_T src_t;

void src_enable(src_param_t *arg)
{
    u32 size;
    size = src_mem_query(SRC_IDAT_LEN, SRC_ODAT_LEN, SRC_FLTB_MAX);
    otp_printf("src mem:%d\n", size);
    cbuf_init(&src_t.src_cbuf, src_cbuffer, SRC_CBUFF_SIZE);
    src_init_api(arg, src_buffer);
    src_t.toggle = 1;
    src_t.empty = 1;
}

void src_disable()
{
    src_exit_api();
    src_t.toggle = 0;
    src_t.empty = 1;
}

u32 src_run(u8 *buf, u16 len)
{
    u32 wlen = 0;
    u8 tmp_buf[SRC_IDAT_LEN];

    if (src_t.toggle) {
        wlen = cbuf_write(&src_t.src_cbuf, buf, len);
        if (wlen != len) {
            putchar('f');
        }
        if (src_t.empty) {
            if (cbuf_get_data_size(&src_t.src_cbuf) >= SRC_KICK_START_LEN) {
                src_t.empty = 0;
                cbuf_read(&src_t.src_cbuf, tmp_buf, SRC_IDAT_LEN);
                //putchar('K');
                src_write_api(tmp_buf, SRC_IDAT_LEN);
            }
        }
    }
    return wlen;
}

void src_kick_start(u8 start_flag)
{
    u8 tmp_buf[SRC_IDAT_LEN];
    //putchar('k');
    if (src_t.toggle && (src_t.empty == 0)) {
        if (start_flag) {
            src_write_api(NULL, SRC_IDAT_LEN);
            return;
        }
        if (cbuf_get_data_size(&src_t.src_cbuf) >= SRC_IDAT_LEN) {
            /* putchar('.'); */
            cbuf_read(&src_t.src_cbuf, tmp_buf, SRC_IDAT_LEN);
            src_write_api(tmp_buf, SRC_IDAT_LEN);
        } else {
            src_t.empty = 1;
        }
    }
}

void src_clear()
{
    src_clear_api();
}

static void src_output_cb(u8 *buf, u16 len, u8 flag)
{
    if (flag & BIT(0)) {
        //output
    }

    //kick start
    if (flag == BIT(0)) {
        src_kick_start(1);/*idat remaind,need not read data */
    } else {
        src_kick_start(0);/*idat done,need read data again*/
    }
}

void src_demo()
{
    src_param_t src_parm;

    src_parm.in_chinc = 1;
    src_parm.in_spinc = 2;
    src_parm.out_chinc = 1;
    src_parm.out_spinc = 2;
    src_parm.in_rate = 44100;
    src_parm.out_rate = 44100;
    src_parm.nchannel = 2;
    src_parm.output_cbk = (void *)src_output_cb;

    src_enable(&src_parm);
    //src_run(NULL, 0);
    //src_disable();

}
#endif


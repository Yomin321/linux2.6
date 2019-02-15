#include "a2dp_sync.h"
#include "string.h"
#include "cbuf/circular_buf.h"
#include "audio/src.h"
#include "audio/dac_api.h"
#include "uart.h"
#include "sdk_cfg.h"


#if A2DP_SYNC_EN

#define SYNC_USE_CBUF	0

typedef struct _A2DP_SYNC {
#if SYNC_USE_CBUF
    cbuffer_t sync_cbuf;
    u8 sync_cbuffer[SRC_IDAT_LEN_MAX * 2];
#endif
    u8 toggle;					/*a2dp sync toggle		*/
    volatile u8 busy;			/*src state				*/
    volatile u16 dump_packets;
    u8 dump_cnt;
    u16 sr_upper_limit;			/*src upper limit value	*/
    u16 sr_lower_limit;			/*src lower limit value	*/

    u16 sr_normal;				/*src normal sr value	*/
    u16 sr_step;				/*src fade step			*/

    u16 sr_step_normal;			/*src recover step		*/
    u16 sr_output;				/*src out_rate			*/
} A2DP_SYNC;
A2DP_SYNC a2dp_sync;

extern cbuffer_t *sbc_decode_get_cbuf();
cbuffer_t *a2dp_dec_cbuf;
#define a2dp_data_len() 		(a2dp_dec_cbuf->data_len)
#define a2dp_data_total_len() 	(a2dp_dec_cbuf->total_len)

/*
***********************************************************************************
*					A2DP SYNC OUTPUT
*
*Description: This function is called when src converted finish
*
*Argument(s): buf = src output
*			  len = src output length
*			  flag = src dma flag
*Returns	: none
*
*Note(s)	:
***********************************************************************************
*/
QLZ(.qlz_a2dp)
void a2dp_sync_output(u8 *buf, u16 len, u8 flag)
{
    u32 wlen = 0;

    if (flag & BIT(0)) {
        wlen = dac_write((s16 *)buf, (u32)len);

        //otp_printf("[%d",get_dac_cbuf_len());
        if (wlen != len) {
            otp_printf("[%d--%d]", wlen, len);
        }
    }

    if (flag == BIT(0)) {
        /*src indat remaind,contine*/
        a2dp_sync.busy = 1;
        src_write_api(NULL, SRC_IDAT_LEN_MAX);
    } else {
        a2dp_sync.busy = 0;
        //__asm__ volatile ("csync");
    }

}
/*
***********************************************************************************
*					SRC INIT
*
*Description: This function is called to init SRC module
*
*Argument(s):
*
*Returns	:
*
*Note(s)	:
***********************************************************************************
*/
s32 src_init(void)
{
    s32 err = 0;
    u32 size;
    src_param_t src_p;

    src_p.in_chinc = 1;
    src_p.in_spinc = 2;
    src_p.out_chinc = 1;
    src_p.out_spinc = 2;
    src_p.in_rate = 44100;
    src_p.out_rate = 44100;
    src_p.nchannel = 2;
    src_p.output_cbk = (void *)a2dp_sync_output;

    size = src_mem_query(SRC_IDAT_LEN_MAX, SRC_ODAT_LEN_MAX, SRC_FLTB_MAX);
    err = src_init_api(&src_p, src_buffer);
    return err;
}

/*
***********************************************************************************
*					A2DP SYNC INIT
*
*Description: This function is called init sync module
*
*Argument(s):
*
*Returns	:
*
*Note(s)	:
***********************************************************************************
*/
s32 a2dp_sync_init()
{
    s32 err = 0;
    a2dp_dec_cbuf = sbc_decode_get_cbuf();
    if (a2dp_dec_cbuf == NULL) {
        return -1;
    }

    memset(&a2dp_sync, 0, sizeof(A2DP_SYNC));

    a2dp_sync.sr_upper_limit = 44150;
    a2dp_sync.sr_lower_limit = 44050;
    a2dp_sync.sr_normal = 44100;
    a2dp_sync.sr_output = 44100;
    a2dp_sync.sr_step = 5;
    a2dp_sync.sr_step_normal = 1;
    a2dp_sync.dump_packets = 0;
    a2dp_sync.dump_cnt = 0;
#if SYNC_USE_CBUF
    cbuf_init(&a2dp_sync.sync_cbuf, &a2dp_sync.sync_cbuffer, sizeof(a2dp_sync.sync_cbuffer));
#endif

    err = src_init();
    if (err == 0) {
        a2dp_sync.toggle = 1;
        /* puts("a2dp_sync_init_OK\n"); */
    }
    return err;
}

/*
***********************************************************************************
*					A2DP SYNC AUTO CONTROL(AC)
*
*Description: This function is called to reset auto control SRC param
*
*Argument(s): input_sr = src input sample_rate
*
*Returns	: none
*
*Note(s)	:
***********************************************************************************
*/
void a2dp_sync_ac(u16 input_sr)
{
    src_param_t src_p;
    static u32 put_cnt = 0;
    static u32 put_cnt1 = 0;

    put_cnt++;
    put_cnt1++;
    if (a2dp_data_len() > 10 * 1024) {
        a2dp_sync.sr_output = ((a2dp_sync.sr_output - a2dp_sync.sr_step) < a2dp_sync.sr_lower_limit) ? \
                              a2dp_sync.sr_lower_limit : (a2dp_sync.sr_output - a2dp_sync.sr_step);
        /* putchar('b'); */
        /* put_u16hex(a2dp_sync.sr_output); */
    } else if (a2dp_data_len() < 8 * 1024) {
        if (put_cnt > 50) {
            put_cnt = 0;
            //putchar('s');
        }
        a2dp_sync.sr_output = ((a2dp_sync.sr_output + a2dp_sync.sr_step) > a2dp_sync.sr_upper_limit) ? \
                              a2dp_sync.sr_upper_limit : (a2dp_sync.sr_output + a2dp_sync.sr_step);
    } else {
        if (put_cnt1 > 50) {
            put_cnt1 = 0;
            //putchar('n');
        }
        if (a2dp_sync.sr_output > a2dp_sync.sr_normal) {
            a2dp_sync.sr_output = ((a2dp_sync.sr_output - a2dp_sync.sr_step_normal) < a2dp_sync.sr_normal) ? \
                                  a2dp_sync.sr_normal : (a2dp_sync.sr_output - a2dp_sync.sr_step_normal);
        } else {
            a2dp_sync.sr_output = ((a2dp_sync.sr_output + a2dp_sync.sr_step_normal) > a2dp_sync.sr_normal) ? \
                                  a2dp_sync.sr_normal : (a2dp_sync.sr_output + a2dp_sync.sr_step_normal);
        }
    }

    if (a2dp_data_len() < 1024) {
        otp_printf("\n\n\n[sbc_data:%d]\n", a2dp_data_len());
    }

    /* otp_printf("[%d]",input_sr); */
    src_p.in_rate = input_sr;
    //src_p.out_rate = 44100;
    src_p.out_rate = a2dp_sync.sr_output;
    /* otp_printf("[%d--%d]",src_p.out_rate,a2dp_data_len()); */
    src_config_api(&src_p);
}

/*
***********************************************************************************
*					A2DP SYNC RUN
*
*Description: This function is called kick start SRC convertion
*
*Argument(s): buf:indat
*			  len:indat_len
*			  sr :a2dp decodec sample_rate
*
*Returns	: wlen:convert len
*
*Note(s)	:
***********************************************************************************
*/
u32 a2dp_sync_run(void *buf, u32 len, u16 sr)
{
    u32 wlen = 0;

    if (a2dp_sync.dump_packets) {
        a2dp_sync.dump_packets--;
        //otp_printf("[%d]",a2dp_sync.dump_packets);
        if ((a2dp_data_len() > 10 * 1024) && (a2dp_sync.dump_cnt < 5) && (a2dp_sync.dump_packets == 0)) {
            a2dp_sync.dump_packets = 40;
            a2dp_sync.dump_cnt++;
        }
        memset(buf, 0x00, len);
    }

    if (a2dp_sync.toggle) {
        while (a2dp_sync.busy) {
            putchar('w');
        };
#if SYNC_USE_CBUF
        u8 tmp_buf[SRC_IDAT_LEN_MAX];
        wlen = cbuf_write(&a2dp_sync.sync_cbuf, buf, len);
        if (wlen != len) {
            otp_printf("[%d-%d]", cbuf_get_data_size(&a2dp_sync.sync_cbuf), len);
        }
        if (cbuf_get_data_size(&a2dp_sync.sync_cbuf) >= SRC_IDAT_LEN_MAX) {
            cbuf_read(&a2dp_sync.sync_cbuf, tmp_buf, SRC_IDAT_LEN_MAX);
            a2dp_sync_ac(sr);
            a2dp_sync.busy = 1;
            src_write_api(tmp_buf, SRC_IDAT_LEN_MAX);
        }
#else
        a2dp_sync_ac(sr);
        a2dp_sync.busy = 1;
        if (len > SRC_IDAT_LEN_MAX) {
            wlen = SRC_IDAT_LEN_MAX;
        } else {
            wlen = len;
        }
        src_write_api(buf, len);

#endif
    } else {
        wlen = dac_write((s16 *)buf, (u32)len);
    }
    return wlen;
}

/*
***********************************************************************************
*					A2DP SYNC EXIT
*
*Description: This function is called when a2dp dec stop
*
*Argument(s): none
*
*Returns	: 0 = success
*			 !0 = false
*
*Note(s)	:
***********************************************************************************
*/
s32 a2dp_sync_exit()
{
    src_exit_api();
    a2dp_sync.toggle = 0;
    /* puts("a2dp_sync_exit_OK\n"); */
    return 0;
}

#endif /* A2DP_SYNC_EN */

#include "sdk_cfg.h"
#include "audio/dac_api.h"
#include "audio/eq_api.h"
#include "audio/eq.h"
#include "crc_api.h"
#include "dev_mg_api.h"
#include "dev_manage.h"
#include "fs.h"
#include "uart_param.h"

#if EQ_EN

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".eq_app_bss")
#pragma data_seg(	".eq_app_data")
#pragma const_seg(	".eq_app_const")
#pragma code_seg(	".eq_app_code")
#endif

static EQ_CFG eq_cfg;
EQ_ARG  eq_arg;
static u8 eq_toggle = 0;
static u8 eq_buffer[704] AT(.dac_buf_sec) __attribute__((aligned(4)));

const int eq_seg_gain[7][10] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 		///<Normal
    { 0, 8, 8, 4, 0, 0, 0, 0, 2, 2}, 		///<Rock
    {-2, 0, 2, 4, -2, -2, 0, 0, 4, 4}, 		///<Pop
    { 4, 2, 0, -3, -6, -6, -3, 0, 3, 5}, 	///<Classic
    { 0, 0, 0, 4, 4, 4, 0, 2, 3, 4}, 		///<Jazz
    {-2, 0, 0, 2, 2, 0, 0, 0, 4, 4}, 		///<Country
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},   		///<user_defined
};

const int eq_global_gain[6] = {
    0, -6, -3, -6, -6, -3
};

static const int eq_filt_44100[] = {
    2085775, -1037283, 5647, 0, -1,
    2051103, -1003829, 22374, 0, -1,
    1905750, -876682, 85947, 0, -1,
    1294532, -488959, 279808, 0, -1,
    -204953, 733664, 891120, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};
//22.05
static const int eq_filt_22050[] = {
    2074352, -1026110, 11233, 0, -1,
    2004391, -960912, 43832, 0, -1,
    1705578, -728669, 159954, 0, -1,
    476389, -91739, 478419, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};
//11.025
static const int eq_filt_11025[] = {
    2051368, -1004115, 22230, 0, -1,
    1909022, -879987, 84294, 0, -1,
    1286901, -479895, 284341, 0, -1,
    -204625, 734221, 891398, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};

//48k
static const int eq_filt_48000[] = {
    2086701, -1038196, 5190, 0, -1,
    2054870, -1007394, 20591, 0, -1,
    1921645, -889651, 79463, 0, -1,
    1363328, -525660, 261458, 0, -1,
    -239791, 568886, 808731, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};
//24.00
static const int eq_filt_24000[] = {
    2076211, -1027918, 10329, 0, -1,
    2012026, -967768, 40404, 0, -1,
    1738642, -751399, 148589, 0, -1,
    600306, -152036, 448270, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};
//12.00
static const int eq_filt_12000[] = {
    2055116, -1007658, 20459, 0, -1,
    1924692, -892724, 77926, 0, -1,
    1355916, -517100, 265738, 0, -1,
    -239623, 569330, 808953, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};

//32k
static const int eq_filt_32000[] = {
    2081461, -1033044, 7766, 0, -1,
    2033518, -987394, 30591, 0, -1,
    1831021, -818317, 115130, 0, -1,
    974346, -329358, 359609, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};

static const int eq_filt_16000[] = {
    2065683, -1017739, 15419, 0, -1,
    1968630, -929579, 59498, 0, -1,
    1549409, -628492, 210042, 0, -1,
    0, 187252, 617914, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};

static const int eq_filt_8000[] = {
    2033866, -987783, 30397, 0, -1,
    1835253, -822632, 112972, 0, -1,
    966373, -318082, 365247, 0, -1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,

};

s32 eq_cfg_read(void)
{
    char *path = NULL;
    s32 ret = 0;
    DEV_HANDLE dev = cache;
#if  AUDIO_EFFECT_HW_EQ
    path = "/eq_cfg_hw.bin";
#endif
#if  AUDIO_EFFECT_SW_EQ
    path = "/eq_cfg_sw.bin";
#endif

    if (path == NULL) {
        return -1;
    }

    otp_printf("read:%s\n", path);
    ret = fs_open_file_bypath(dev, &eq_cfg, sizeof(EQ_CFG), path);
    if (ret == false) {
        puts("open eq cfg file error!\n");
        return -1;
    }
    puts("eq_cfg_read OK\n");
    return 0;
}

static s32 eq_cfg_check(void)
{
    //put_buf((u8 *)&eq_cfg, sizeof(EQ_CFG));
    if ((eq_cfg.crc16 == crc16((u8 *)&eq_cfg, sizeof(EQ_CFG) - 4)) && (eq_cfg.crc16 != 0)) {
        puts("EQ cfg crc ok\n");
        return 0;
    } else {
        puts("EQ cfg crc err\n");
        return -1;
    }
}


void eq_init(void)
{
    int eq_mem;
    s32 ret = 0;
    eq_mem = eq_mem_query(EQ_CHANNEL_NUM, EQ_SECTION_NUM);/*nChannel,nsection*/
    otp_printf("eq_mem:%d\n", eq_mem);
    ret = eq_cfg_check();
    eq_init_api(eq_buffer);
    if (ret == 0) {
        eq_config_api((int *)eq_cfg.eq_filt_0,
                      (int *)eq_cfg.eq_filt_1,
                      (int *)eq_cfg.eq_filt_2,
                      (int *)eq_cfg.eq_filt_3,
                      (int *)eq_cfg.eq_filt_4,
                      (int *)eq_cfg.eq_filt_5,
                      (int *)eq_cfg.eq_filt_6,
                      (int *)eq_cfg.eq_filt_7,
                      (int *)eq_cfg.eq_filt_8,
                      (int (*)[10])eq_cfg.eq_freq_gain,
                      (int *)eq_cfg.global_gain,
                      1);
        eq_arg.mode = eq_cfg.eq_type;
        /* put_buf((u8 *)eq_cfg.eq_freq_gain[6], 40); */
        otp_printf("eq_arg.mode:%d\n", eq_arg.mode);

    } else {
        eq_config_api((int *)eq_filt_44100,
                      (int *)eq_filt_22050,
                      (int *)eq_filt_11025,
                      (int *)eq_filt_48000,
                      (int *)eq_filt_24000,
                      (int *)eq_filt_12000,
                      (int *)eq_filt_32000,
                      (int *)eq_filt_16000,
                      (int *)eq_filt_8000,
                      (int (*)[10])eq_seg_gain,
                      (int *)eq_global_gain,
                      1);
        eq_arg.mode = EQ_NORMAL;
    }
    /* eq_init_api(eq_buffer); */
    eq_enable();

}

void eq_enable(void)
{
    eq_mode_api(eq_arg.mode);
    eq_toggle = 1;
    puts("eq enable\n");
}

void eq_disable(void)
{
    puts("eq_disable\n");
    if (0 == eq_toggle) {
        return;
    }
    eq_dis_api();
    eq_toggle = 0;
}

AT_AUDIO
void eq_run(short *in, short *out, int npoint)
{
    eq_run_api(in, out, npoint);
}

void eq_mode_set(u8 mode)
{
    eq_mode_api(mode);
}

void eq_samplerate(u16 sr)
{
    if (eq_toggle) {
        eq_samplerate_api(sr);
    }
}


void hw_eq_cfg_update(void)
{
    int need_buf;
    s32 ret = 0;
    puts("hw_eq_update\n");
    /* otp__printf("eq_type:%d\n", eq_cfg.type); */
    /* otp__printf("seg_num:%d\n", eq_cfg.seg_num); */
    eq_init_api(eq_buffer);
    eq_config_api((int *)eq_cfg.eq_filt_0,
                  (int *)eq_cfg.eq_filt_1,
                  (int *)eq_cfg.eq_filt_2,
                  (int *)eq_cfg.eq_filt_3,
                  (int *)eq_cfg.eq_filt_4,
                  (int *)eq_cfg.eq_filt_5,
                  (int *)eq_cfg.eq_filt_6,
                  (int *)eq_cfg.eq_filt_7,
                  (int *)eq_cfg.eq_filt_8,
                  (int (*)[10])eq_cfg.eq_freq_gain,
                  (int *)eq_cfg.global_gain,
                  1);
    eq_arg.mode = eq_cfg.eq_type;
    eq_enable();
}

#if EQ_UART_DEBUG
#define EQ_PACKET_MAX_IDX  4      //4+1 = 5 packet.
#define EQ_PACKET_LEN      512
extern void sw_eq_cfg_update(void);
extern void hw_eq_cfg_update(void);
u8 update_eq_info(void *new_eq_info, u32 size, u8 packet_idx)
{
    EQ_CFG *eq_cfg_buf = &eq_cfg;
    if (0 == packet_idx) {
        memcpy((u8 *)eq_cfg_buf, new_eq_info, size);
    } else {
        memcpy(((u8 *)eq_cfg_buf) + (EQ_PACKET_LEN - 4) * packet_idx, new_eq_info, size);
    }

    if (EQ_PACKET_MAX_IDX == packet_idx) {
        if (eq_cfg_buf->crc16 == crc16(eq_cfg_buf, sizeof(EQ_CFG) - 4)) {
            puts("recv new EQ, crc ok\n");
#if AUDIO_EFFECT_HW_EQ
            hw_eq_cfg_update();
#elif AUDIO_EFFECT_SW_EQ
            sw_eq_cfg_update();
#endif
            return 1;
        } else {
            puts("recv new EQ, crc Err!!!\n");
        }
    }

    return 0;
}

void eq_uart_debug_write(char a);
void eq_uart_debug_isr_callback(u8 uto_buf, void *p, u8 isr_flag)
{
    u8 packet_idx = 0;
    u8 *rx_buf = p;
    u8 res = 0;
    //otp_printf("[%d] %d", isr_flag, uto_buf);

    if ((UART_ISR_TYPE_DATA_COME == isr_flag) && ('E' == rx_buf[0] && 'Q' == rx_buf[1])) {
        packet_idx = rx_buf[2];
        /* otp_printf("[%d]", packet_idx); */
        if (EQ_PACKET_MAX_IDX == packet_idx) {
            res = update_eq_info(rx_buf + 4, 2140 - 4 * EQ_PACKET_LEN - 4, packet_idx);
        } else if (packet_idx < EQ_PACKET_MAX_IDX) {
            res = update_eq_info(rx_buf + 4, EQ_PACKET_LEN - 4, packet_idx);
        }

        if (EQ_PACKET_MAX_IDX - 1 == packet_idx) { //packet 3
            JL_UART1->RXCNT = 2140 - 4 * EQ_PACKET_LEN;
            JL_UART1->RXSADR = (u32)rx_uart1_buf;
            JL_UART1->RXEADR = (u32)(rx_uart1_buf + ut_dma_wr_cnt[1]);
        } else {
            JL_UART1->RXCNT = (u32)ut_dma_wr_cnt[1];
            JL_UART1->RXSADR = (u32)rx_uart1_buf;
            JL_UART1->RXEADR = (u32)(rx_uart1_buf + ut_dma_wr_cnt[1]);
        }
        if (res || (EQ_PACKET_MAX_IDX == packet_idx)) { //all packet recv ok
            if (res) {
                puts("send ok\n");
                eq_uart_debug_write('O');
                eq_uart_debug_write('K');
            } else {
                puts("send err\n");
                eq_uart_debug_write('E');
                eq_uart_debug_write('R');

            }
            JL_UART1->RXCNT = (u32)ut_dma_wr_cnt[1];
            JL_UART1->RXSADR = (u32)rx_uart1_buf;
            JL_UART1->RXEADR = (u32)(rx_uart1_buf + ut_dma_wr_cnt[1]);
        }
    }

    if (UART_ISR_TYPE_TIMEOUT == isr_flag) {
        puts("eq uart timeout!!!\n");
        JL_UART1->RXCNT = (u32)ut_dma_wr_cnt[1];
        JL_UART1->RXSADR = (u32)rx_uart1_buf;
        JL_UART1->RXEADR = (u32)(rx_uart1_buf + ut_dma_wr_cnt[1]);
    }
    JL_UART1->OTCNT = 20000 * 1000;
}

#endif
#else
s32 eq_cfg_read(void)
{
    return 0;
}
void eq_init(void) {}
void eq_mode_set(u8 mode) {}
#endif


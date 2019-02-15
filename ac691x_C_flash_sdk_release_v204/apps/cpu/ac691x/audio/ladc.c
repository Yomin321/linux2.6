/*
*********************************************************************************************************
*                                             BR17
*
*                                             CODE
*
*                          (c) Copyright 2015-2016, ZHUHAI JIELI
*                                           All Rights Reserved
*
* File : *
* By   : jamin.li
* DATE : 11/11/2015 build this file
*********************************************************************************************************
*/
#include "sdk_cfg.h"
#include "cbuf/circular_buf.h"
#include "audio/dac.h"
#include "audio/ladc.h"
#include "audio/dac_api.h"
#include "audio/linein_api.h"
#include "audio/alink.h"
#include "cpu/audio_param.h"
#include "aec/aec.h"
#include "audio/pdm_link.h"
#include <stdlib.h>
#include "sys_detect.h"
#include "dev_pc.h"
#include "rec_api.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".ladc_app_bss")
#pragma data_seg(	".ladc_app_data")
#pragma const_seg(	".ladc_app_const")
#pragma code_seg(	".ladc_app_code")
#endif

#define LADC_DATA_2_DAC_DBG		0

/*
 *********************************************************************************************************
 *
 * Description: calculate data energy
 * Arguments  : buffer = source_data
 *				len = data length(byte)
 *				packet_cnt = total calculate packets
 * Returns    : none
 *
 * Note:
 *********************************************************************************************************
 */
u32 data_energy_value(void *buffer, u16 len, u16 packet_cnt)
{
    static u32 total_res = 0;
    static u32 digital_energy = 0;
    static u16 cnt = 0;
    u32 res = 0;
    u32 i;

    s16 *buf = buffer;
    len >>= 1;//convert to point

    for (i = 0; i < len; i++) {
        res += abs(buf[i]);
    }
    total_res += res;

    cnt++;
    if (cnt >= packet_cnt) {
        cnt = 0;
        digital_energy = total_res;
        total_res = 0;
    }
    return digital_energy;
}

/*
 *********************************************************************************************************
 *
 * Description: digital_volume_ctl
 * Arguments  : buffer
 *				point_len
 *				ch = 1(mono),2(stereo)
 * Returns    : none
 *
 * Note:
 *********************************************************************************************************
 */
extern const u16 digital_vol_tab[MAX_DIGITAL_VOL_L + 1];
volatile u8 digital_vol = 31;
void digital_volume_ctl(void *buffer, u16 point_len, u8 ch)
{
    s32 valuetemp;
    u32 i;
    u16 curtabvol, curtabvor;

    s16 *buf = buffer;

    //get digital_vol
    curtabvol = digital_vol_tab[digital_vol];
    curtabvor = curtabvol;

    if (ch == 2) {
        //stereo
        for (i = 0; i < point_len; i += 2) {
            ///left channel
            valuetemp = buf[i];
            valuetemp = (valuetemp * curtabvol) >> 14 ;
            if (valuetemp < -32768) {
                valuetemp = -32768;
            } else if (valuetemp > 32767) {
                valuetemp = 32767;
            }
            buf[i] = (s16)valuetemp;

            ///right channel
            valuetemp = buf[i + 1];
            valuetemp = (valuetemp * curtabvor) >> 14 ;
            if (valuetemp < -32768) {
                valuetemp = -32768;
            } else if (valuetemp > 32767) {
                valuetemp = 32767;
            }
            buf[i + 1] = (s16)valuetemp;
        }
    } else {
        //mono
        for (i = 0; i < point_len; i++) {
            valuetemp = buf[i];
            valuetemp = (valuetemp * curtabvol) >> 14 ;
            if (valuetemp < -32768) {
                valuetemp = -32768;
            } else if (valuetemp > 32767) {
                valuetemp = 32767;
            }
            buf[i] = (s16)valuetemp;
        }
    }
}

/*
 *********************************************************************************************************
 *
 * Description: ladc to dac
 * Arguments  : buffer
 *				point_len
 * Returns    : none
 *
 * Note:
 *********************************************************************************************************
 */
#if 0
static const u8 sine_buf_32K[] = {

    0x00, 0x00, 0xae, 0x11, 0xad, 0x22, 0x58, 0x32,
    0x13, 0x40, 0x58, 0x4b, 0xb8, 0x53, 0xe0, 0x58,
    0x9e, 0x5a, 0xe0, 0x58, 0xb8, 0x53, 0x58, 0x4b,
    0x13, 0x40, 0x58, 0x32, 0xad, 0x22, 0xae, 0x11,
    0x00, 0x00, 0x52, 0xee, 0x53, 0xdd, 0xa8, 0xcd,
    0xed, 0xbf, 0xa8, 0xb4, 0x48, 0xac, 0x20, 0xa7,
    0x62, 0xa5, 0x20, 0xa7, 0x48, 0xac, 0xa8, 0xb4,
    0xed, 0xbf, 0xa8, 0xcd, 0x53, 0xdd, 0x52, 0xee
};
static const u8 sine_buf_8K[] = {

    0x00, 0xad, 0x13, 0xb8, 0x9e, 0xb8, 0x13, 0xad,
    0x00, 0x53, 0xed, 0x48, 0x62, 0x48, 0xed, 0x53,
    0x00, 0xad, 0x13, 0xb8, 0x9e, 0xb8, 0x13, 0xad,
    0x00, 0x53, 0xed, 0x48, 0x62, 0x48, 0xed, 0x53,
    0x00, 0xad, 0x13, 0xb8, 0x9e, 0xb8, 0x13, 0xad,
    0x00, 0x53, 0xed, 0x48, 0x62, 0x48, 0xed, 0x53,
    0x00, 0xad, 0x13, 0xb8, 0x9e, 0xb8, 0x13, 0xad,
    0x00, 0x53, 0xed, 0x48, 0x62, 0x48, 0xed, 0x53
};
#endif

void ladc_to_dac(void *buf, u32 len)
{

    u8 cnt;
    s16 l2d_buf[DAC_DUAL_BUF_LEN];
    s16 *sp = buf;					/*src data	*/
    //sp = (void*)sine_buf_8K; 		/*debug		*/
    s16 *dp = (s16 *)l2d_buf;		/*dst data	*/

    for (cnt = 0; cnt < (len / 2); cnt++) {
        dp[cnt * 2] = sp[cnt];
        dp[cnt * 2 + 1] = sp[cnt];
    }
    dac_write(dp, len << 1);
}

void ladc_stereo_to_dac(void *buf_l, void *buf_r, u32 len)
{
    s16 l2d_buf[DAC_DUAL_BUF_LEN];
    s16 *sp_l = buf_l;
    s16 *sp_r = buf_r;
    s16 *dp = l2d_buf;
    u8 cnt;

    for (cnt = 0; cnt < (len / 2); cnt++) {
        dp[cnt * 2] = sp_l[cnt];
        dp[cnt * 2 + 1] = sp_r[cnt];
    }
    dac_write(dp, len << 1);
}

/*
 *********************************************************************************************************
 *
 * Description: ladc_isr_callback
 * Arguments  : ladc_buf,ladc sample rate buf
 *				buf_flag,indicate the using buf
 *				buf_len,ladc buf length(default:DAC_SAMPLE_POINT * 2)
 * Returns    : none
 *
 * Note		  :
 *********************************************************************************************************
 */
void ladc_isr_callback(void *ladc_buf, u32 buf_flag, u32 buf_len)
{
    s16 *ladc_mic;

    ladc_mic = (s16 *)ladc_buf;
    ladc_mic = ladc_mic + (buf_flag * DAC_SAMPLE_POINT);

    //calculate energy
    //data_energy_value(ladc_mic,DAC_DUAL_BUF_LEN,1);
    //putchar('a');

#if LADC_DATA_2_DAC_DBG
    ladc_to_dac(ladc_mic, DAC_DUAL_BUF_LEN);
#endif

#if USB_PC_EN
    usb_slave_mic_input(ladc_mic, DAC_SAMPLE_POINT);
#endif

#if REC_EN
    rec_ladc_data_cb(ladc_mic, DAC_DUAL_BUF_LEN);
#endif

    if (aec_interface.fill_adc_ref_buf) {
        aec_interface.fill_adc_ref_buf(ladc_mic, DAC_DUAL_BUF_LEN);
    }
}

//*********************************************************
//                        LADC API
//*********************************************************
void ladc_reg_init(u16 sr)
{
    //puts("ladc_reg_init\n");
    ladc_isr_cb_register(ladc_isr_callback);
    ladc_enable(sr, VCOMO_EN, LADC_MIC_CHANNEL, LADC_CFG);
}

void ladc_close()
{
    //puts("ladc_close\n");
    ladc_disable();
}


//*********************************************************
//                        DAA API
//*********************************************************
void microphone_open(u8 mic_gain, u8 mic_gx2)
{
    //puts("microphone_open\n");

    dac_mute(1, 0);


    ladc_adcude(1);
    ladc_mic_gain(mic_gain, mic_gx2);
    ladc_mic_en(1);
    mic_2_dac(1, 1);
    dac_mute(0, 0);
    ladc_adcude(0);
}

void microphone_close(void)
{
    //puts("microphone_close\n");
    ladc_mic_en(0);
    mic_2_dac(0, 0);
}

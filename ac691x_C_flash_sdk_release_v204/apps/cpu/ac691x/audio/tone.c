#include "audio/dac.h"
#include "audio/tone.h"
#include "sdk_cfg.h"
#include "cpu/audio_param.h"
#include "audio/dac_api.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".tone_app_bss")
#pragma data_seg(	".tone_app_data")
#pragma const_seg(	".tone_app_const")
#pragma code_seg(	".tone_app_code")
#endif

#if TONE_EN

#define TONE_FADE_OUT_EN		1

///key voice table for 44.1KHz, 16 * 16bit
#define tone_var_44K_TAB_MAX	16
const s16 tone_var_44100Hz_tab[tone_var_44K_TAB_MAX] = {
    0,
    10922,
    20181,
    26367,
    28540,
    26367,
    20181,
    10922,
    0,
    -10922,
    -20181,
    -26367,
    -28540,
    -26367,
    -20181,
    -10922,
};

///key voice table for 48KHz, 18 * 16bit
#define tone_var_48K_TAB_MAX	18
const s16 tone_var_48000Hz_tab[tone_var_48K_TAB_MAX] = {
    0,
    9761,
    18345,
    24716,
    28106,
    28106,
    24716,
    18345,
    9761,
    0,
    -9761,
    -18345,
    -24716,
    -28106,
    -28106,
    -24716,
    -18345,
    -9761
};
//key voice table for 32KHz, 12 * 16bit
#define tone_var_32K_TAB_MAX	12
const s16 tone_var_32000Hz_tab[tone_var_32K_TAB_MAX] = {
    16384,
    28378,
    32767,
    28378,
    16384,
    0,
    -16384,
    -28378,
    -32767,
    -28378,
    -16384,
    -1
};

SIN_TONE_VAR sin_tone_var;

void tone_init(void)
{
    memset(&sin_tone_var, 0, sizeof(SIN_TONE_VAR));
    sin_tone_var.vol = 1500;	/*default tone vol*/
    tone_toggle(TONE_EN);
}

void tone_toggle(u16 en)
{
    sin_tone_var.toggle = en;	/*default tone vol*/
}

u16 is_tone_busy(void)
{
    return sin_tone_var.tab_cnt;
}

void tone_sr_set(u16 sr)
{
    sin_tone_var.dac_sr = sr;
}

void tone_play(u16 cnt)
{
    u8 step = 0;

    if (0 == sin_tone_var.toggle) {
        return;
    }

    dac_ctl.keyvoice_check = 0;
    //dac_automute_init(AUTO_MUTE_CFG,-1, -1,-1);

    if (sin_tone_var.tab_cnt) {
        return;
    }

    dac_toggle(1);

    switch (sin_tone_var.dac_sr) {
    case 12000://step = 4
        step += 2;
    case 24000://step = 2
        step++;
    case 48000://step = 1
        step++;
        sin_tone_var.sr_tab = tone_var_48000Hz_tab;
        sin_tone_var.sr_tab_size = tone_var_48K_TAB_MAX;
        break ;

    case 11025:
        step += 2;
    case 22050:
        step++;
    case 44100:
        step++;
        sin_tone_var.sr_tab = tone_var_44100Hz_tab;
        sin_tone_var.sr_tab_size = tone_var_44K_TAB_MAX;
        break ;

    case 8000:
        step += 2;
    case 16000:
        step++;
    case 32000:
        step++;
        sin_tone_var.sr_tab = tone_var_32000Hz_tab;
        sin_tone_var.sr_tab_size = tone_var_32K_TAB_MAX;
        break ;

    default:
        otp_printf("tone_dac_sr:%d\n", sin_tone_var.dac_sr);
        return ;
    }

    sin_tone_var.step = step;
    sin_tone_var.tab_cnt = cnt;
    sin_tone_var.point_cnt = 0;
    dac_ctl.keyvoice_check = 1;
}

/*
***********************************************************************************
*							ADD TONE TO DAC
*
*Description: This function called to add tone to dac buf
*
*Argument(s): buff	dac raw data
*			  len	dac raw data len
*
*Returns	: none
*
*Note(s)	:
***********************************************************************************
*/
void tone_2_dac(s16 *buff, u32 len)
{
    u16 tmp_tone_vol;
    if (sin_tone_var.tab_cnt) {
        u32 i = 0;
        tmp_tone_vol = sin_tone_var.vol;
        while (i < len) {
            s32 tmp32;
            s16 tmp16;

#if TONE_FADE_OUT_EN
            /*tone fade_out*/
#define FADE_OUT_TAB_CNT	3
#define FADE_OUT_STEP		50
            if (sin_tone_var.tab_cnt <= FADE_OUT_TAB_CNT) {
                if (tmp_tone_vol > FADE_OUT_STEP) {
                    tmp_tone_vol = tmp_tone_vol - FADE_OUT_STEP;
                } else {
                    tmp_tone_vol = 0;
                }
                //printf("<%d>",tmp_tone_vol);
            }
#endif

            tmp16  = buff[i];
            tmp32  = (sin_tone_var.sr_tab[sin_tone_var.point_cnt] * sin_tone_var.vol) >> 14;/*16384*/
            /* tmp32  = (sin_tone_var.sr_tab[sin_tone_var.point_cnt]); */
            tmp32 += tmp16;
            //  printf("tmp 32 %d  %d ", tmp32, tone_var.point_cnt);

            if (tmp32 < -32768) {
                tmp32 = -32768;
            } else if (tmp32 > 32767) {
                tmp32 = 32767;
            }
            buff[i++] = tmp32;

            sin_tone_var.dac_lr++;
            if (sin_tone_var.dac_lr > 1) {
                sin_tone_var.dac_lr = 0;
                sin_tone_var.point_cnt += sin_tone_var.step;
                if (sin_tone_var.point_cnt >= sin_tone_var.sr_tab_size) {
                    sin_tone_var.tab_cnt--;
                    sin_tone_var.point_cnt = sin_tone_var.point_cnt % sin_tone_var.sr_tab_size;
                    if (0 == sin_tone_var.tab_cnt) {
                        break;
                    }
                }
            }
        }/*end of while*/

        if (sin_tone_var.tab_cnt == 0) {
            puts("tone_play_end\n");
        }
    }
}

#else

void tone_init(void) {}
void tone_toggle(u16 en) {}
void tone_sr_set(u16 sr) {}
void tone_play(u16 cnt) {}
void tone_2_dac(s16 *buff, u32 len) {}

#endif


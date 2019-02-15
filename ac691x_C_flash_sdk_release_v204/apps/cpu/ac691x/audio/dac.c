/*******************************************************************************************
 File Name: dac.c

 Version: 1.00

 Discription:


 Author:yulin deng

 Email :flowingfeeze@163.com

 Date:2014-01-13 17:00:21

 Copyright:(c)JIELI  2011  @ , All Rights Reserved.
 *******************************************************************************************/
#include "sdk_cfg.h"
#include "cbuf/circular_buf.h"
#include "key_drv/key_voice.h"
#include "common/app_cfg.h"
#include "common/common.h"

#include "cpu/audio_param.h"
#include "audio/dac_api.h"
#include "audio/dac.h"
#include "audio/eq.h"
#include "audio/ladc.h"
#include "audio/tone.h"
#include "audio/src.h"
#include "audio/sin_tab.h"
#include "timer.h"

#include "aec/aec.h"
#include "music_decoder.h"
#include "dec/warning_tone.h"
//#include "bt_tone.h"
#include "flash_api.h"
#include "nv_mem.h"
#include "memory_api.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".dac_app_bss")
#pragma data_seg(	".dac_app_data")
#pragma const_seg(	".dac_app_const")
#pragma code_seg(	".dac_app_code")
#endif

DAC_CTL dac_ctl;

/*----------------------------------------------------------------------------*/
/** @brief:数字音量表，必须通过注册函数注册到库内，
                而且长度必须是DIGITAL_VOL_LIMIT+1
    @param:
    @return:
    @author:Juntham
    @note:
*/
/*----------------------------------------------------------------------------*/
const u16 digital_vol_tab[MAX_DIGITAL_VOL_L + 1] AT_AUDIO = {
    0	,
    93	,
    111	,
    132	,
    158	,
    189	,
    226	,
    270	,
    323	,
    386	,  //9
    462	,
    552	,
    660	,
    789	,
    943	,
    1127,
    1347,
    1610,
    1925,
    2301,  //19
    2751,
    3288,
    3930,
    4698,
    5616,
    6713,
    8025,
    9592,
    11466,
    15200, //29
    16000, //30
    16384  //31
};

/*----------------------------------------------------------------------------*/
/** @brief:  DAC IE control
    @param:
    @return: none
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
void dac_int_enable(void)
{
    dac_ctl.read_able = 0;
    dac_ie_en(1);
    dac_mute(0, 1);
}
void dac_int_disable(void)
{
    dac_mute(1, 1);
    dac_ie_en(0);
}

/*----------------------------------------------------------------------------*/
/** @brief:  DAC SR control
    @param:
    @return: none
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
void dac_sr_cb(u16 rate)
{
    otp_printf("dac_sr = %d\n", rate);
#if EQ_EN
    eq_samplerate(rate);
#endif

    tone_sr_set(rate);
}

/*----------------------------------------------------------------------------*/
/** @brief: DAC write callback
    @param:	buf,data will be written to dac
			len,data len
    @return:none
    @author:
    @note:	1)This function will be called before data been write to dac buf,
*/
/*----------------------------------------------------------------------------*/
u32 dac_write_cb(s16 *buf, u32 len)
{
    return 0;
}

/*----------------------------------------------------------------------------*/
/** @brief : dac_busy_cb
    @param : none
    @return: 0(idle),1(busy)
    @author:
    @note  : 1)This function called to check if dac idle
*/
/*----------------------------------------------------------------------------*/
u8 dac_busy_cb(void)
{
    return 0;
}

/*----------------------------------------------------------------------------*/
/** @brief:  DAC power on delay
    @param:	 mode=0,power on slowly,about 600ms(VCM_RSEL_0,depend on vcom cap)
			 mode=1,power on quickly,about 140ms(VCM_RSEL_1,depend on vcom cap)
			 mode=2,power off delay,about 120ms
			 mode=3,power off delay(vcom NULL)
    @return: none
    @author:
    @note:	vcom_cap	pwr_on_time
			204			320ms
			474			800ms(default)
			105			1.3s
			vcom_cap bigger,noise better when switch dac toggle,but need to delay
			more times.
*/
/*----------------------------------------------------------------------------*/
void dac_delay_cb(u8 mode)
{
    switch (mode) {
    case 0:
        /* puts("dac_on slowly:600ms\n"); */
        delay_2ms(400);
        break;
    case 1:
        /* puts("dac_on quickly:140ms\n"); */
        delay_2ms(100);
        break;
    case 2:
        /* puts("dac_off_delay:120ms\n"); */
        //delay_2ms(60);
        dac_ctl.dac_off_delay = 200;
        break;
    case 3:
        dac_ctl.dac_off_delay = 5;
    default:
        /* otp_printf("dac_delay_cb err:%d\n", mode); */
        break;
    }
}
/*----------------------------------------------------------------------------*/
/** @brief : dac_automute_cb
    @param : statue,automute status
    @return: none
    @author:
    @note  : 1)This function called to get automute status
*/
/*----------------------------------------------------------------------------*/
void dac_automute_cb(u8 status)
{
    if (status) {
        puts(">>auto_mute\n");
    } else {
        puts(">>auto_umute\n");
    }
}

/*----------------------------------------------------------------------------*/
/** @brief:  DAC cbuffer check
    @param:
    @return: none
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
AT_AUDIO
int dac_cbuf_enough(void)
{
    if (get_dac_cbuf_len() >= OUTPUT_BUF_SIZE / 2) {
        return 1;	/*enough*/
    } else {
        return 0;	/*un_enough*/
    }
}

/*----------------------------------------------------------------------------*/
/** @brief:  DAC Differential output
    @param:
    @return:
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
AT_AUDIO
void dac_different_output(s16 *buf, u16 len)
{
#if 0
    u8 i;
    s16 *s16_ptr;
    s32 temp_dac_buf;
    s16_ptr = buf;
    for (i = 0; i < len; i = i + 2) {
        temp_dac_buf = -s16_ptr[i];
        if (temp_dac_buf > 32767) {
            temp_dac_buf = 32767;
        } else if (temp_dac_buf < -32768) {
            temp_dac_buf = -32768;
        }
        s16_ptr[i] = (s16)temp_dac_buf;
    }
#else

    u32 i = 0;
    s16 *dat = buf;
    s16 tmp1, tmp2;

    while (i < len) {
        tmp1 = dat[i];
        tmp2 = dat[i + 1];
        tmp1 = ((s32)tmp1 + tmp2) >> 1;
        dat[i] = tmp1;
        dat[i + 1] = (tmp1 == -32768) ? 32767 : -tmp1;
        i += 2;
    }
#endif
}

/*----------------------------------------------------------------------------*/
/** @brief:  DAC DEBUG
    @param:
    @return:
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
void dac_debug(s16 *dac_buf)
{
#if 0
    //32k
    memcpy(dac_buf, sin_32K, 128);
    return;
#else
    //44.1k
    static u16 sincnt;
    unsigned char i;
    s16 *buffer;
    buffer = dac_buf;
    for (i = 0; i <= 63;) {
        buffer[i] = sin_441k[sincnt];
        i ++;
        buffer[i] = sin_441k[sincnt];
        i ++;
        JL_PORTA->DIR |= BIT(9);
        JL_PORTA->PU |= BIT(9);
        if (!(JL_PORTA->IN & BIT(9))) {
            buffer[i - 2] = 0;  //-220
            buffer[i - 1] = 0;  //-220
        }
        if ((++sincnt) >= 441) {
            sincnt = 0;
        }
    }
#endif
}

/*----------------------------------------------------------------------------*/
/** @brief:  Digital Volume Adjust Function
    @param:
    @return: none
    @author:
    @note:	这个函数可以用来直接对数据进行运算，具体放大还是缩小，你说了算。
			我想当你有这个需求的时候，聪明如你，应该知道这个函数怎么使用。
			默认直接把数据左移一位，即放大2倍。
			比如通话的时候觉得底噪大，可以调小模拟增益，然后通过该函数对数据
			进行放大，从而让声音听起来ok而底噪也得到优化
			如果调用了该函数，后面的digital_vol_ctrl也就可以不调用了，这两个接口
			一样的，现在只是放到外面而已
*/
/*----------------------------------------------------------------------------*/
AT_AUDIO
void digital_vol_ext(void *buffer, u32 len)
{
    s32 valuetemp;
    u32 i;

    u16 curtabvol;
    u16 curtabvor;

    s16 *buf = buffer;
    len >>= 1;

    //curtabvol = get_digital_tab_vol(digital_vol_l, 0);
    //curtabvor = get_digital_tab_vol(digital_vol_r, 1);

    for (i = 0; i < len; i += 2) {
        ///left channel
        valuetemp = buf[i];
        //valuetemp = (valuetemp * curtabvol) >> 14 ;
        valuetemp = valuetemp << 1;
        if (valuetemp < -32768) {
            valuetemp = -32768;
        } else if (valuetemp > 32767) {
            valuetemp = 32767;
        }
        buf[i] = (s16)valuetemp;

        ///right channel
        valuetemp = buf[i + 1];
        //valuetemp = (valuetemp * curtabvor) >> 14 ;
        valuetemp = valuetemp << 1 ;
        if (valuetemp < -32768) {
            valuetemp = -32768;
        } else if (valuetemp > 32767) {
            valuetemp = 32767;
        }
        buf[i + 1] = (s16)valuetemp;
    }
}
/*----------------------------------------------------------------------------*/
/** @brief:  DAC isr callback function
    @param:
    @return: none
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
s16 read_buf[DAC_DUAL_BUF_LEN] AT(.dac_buf_sec)  __attribute__((aligned(4)));

AT_AUDIO
void dac_isr_cb(void *dac_buf, u8 buf_flag)
{
    //putchar('d');
#if 0
    dac_debug(dac_buf);
    return;
#endif

    /* JL_PORTA->DIR &=~BIT(11); */
    /* JL_PORTA->OUT ^= BIT(11); */

    /*******************************************************************/
    /* WARNING: dac_isr_cb内调用得所有函数和常数都必须使用AT_AUDIO定义 */
    /*******************************************************************/

    if (0 == dac_ctl.read_able) {
        if (dac_cbuf_enough() == 0)	{
            memset(dac_buf, 0x00, DAC_BUF_LEN);
            if (0 == get_vm_statu()) {
                tone_2_dac((s16 *)dac_buf, DAC_SAMPLE_POINT * 2);
                dac_digit_energy_value(dac_buf, DAC_BUF_LEN);
#if DAC_DIFF_OUTPUT
                dac_different_output(dac_buf, DAC_DUAL_BUF_LEN);
#endif
                if (aec_interface.fill_dac_echo_buf) {
                    aec_interface.fill_dac_echo_buf((s16 *)read_buf, DAC_SAMPLE_POINT << 1);
                }
                music_decoder_loop_resume();
//            putchar('N');
            }
            return;
        }
        dac_ctl.read_able = 1;
    }

    dac_hp_toggle(DAC_CHANNEL_SLECT);
    if (dac_ctl.read_able) {
        if (dac_read(read_buf, DAC_BUF_LEN) != DAC_BUF_LEN) {
            //putchar('N');
            if (get_dac_cbuf_len()) {
                memset(read_buf, 0x00, DAC_BUF_LEN);
                dac_read(read_buf, get_dac_cbuf_len());
            }
            dac_ctl.read_able = 0;
        }
    }

    if (0 == get_vm_statu()) {
        tone_2_dac((s16 *)read_buf, DAC_SAMPLE_POINT * 2);
    }

    /*******************************************************************/
    /* WARNING: dac_isr_cb内调用得所有函数和常数都必须使用AT_AUDIO定义 */
    /*******************************************************************/


    if (is_dac_mute()) {
        memset(read_buf, 0x00, DAC_BUF_LEN);
    }

#if DAC_SOUNDTRACK_COMPOUND
    dac_digital_lr_add(read_buf, DAC_BUF_LEN);
#endif

#if DAC_DIFF_OUTPUT
    dac_different_output(read_buf, DAC_DUAL_BUF_LEN);
#endif

    dac_digit_energy_value(read_buf, DAC_BUF_LEN);

#if (CALL_USE_DIFF_OUTPUT && VCOMO_EN)
    if (aec_interface.aec_run) {
        dac_different_output(read_buf, DAC_DUAL_BUF_LEN);
        //digital_vol_ext(read_buf, DAC_BUF_LEN);
    }
#endif
    digital_vol_ctrl(read_buf, DAC_BUF_LEN);

    if ((0 == get_vm_statu()) && aec_interface.fill_dac_echo_buf) {
        aec_interface.fill_dac_echo_buf((s16 *)read_buf, DAC_SAMPLE_POINT << 1);
    }

#if EQ_EN
    eq_run((short *)read_buf, (short *)dac_buf, DAC_SAMPLE_POINT);
#else
    memcpy(dac_buf, read_buf, DAC_BUF_LEN);
#endif

    /*******************************************************************/
    /* WARNING: dac_isr_cb内调用得所有函数和常数都必须使用AT_AUDIO定义 */
    /*******************************************************************/


    if ((0 == get_vm_statu()) && is_dac_write_able(OUTPUT_BUF_LIMIT) != 0) {
        music_decoder_loop_resume();
//        putchar('4');
    }
}

/*----------------------------------------------------------------------------*/
/** @brief: Audio Moudule initial
    @param:
    @return:
    @author:Juntham
    @note:
*/
/*----------------------------------------------------------------------------*/
static u32 dac_buf[2][DAC_SAMPLE_POINT] AT(.dac_buf_sec);	/* dac DMA buf		*/
static u8 dac_cbuf[OUTPUT_BUF_SIZE] 	AT(.dac_buf_sec);	/* dac cbuf			*/
void audio_buf_init(void)
{
    /* puts("audio buf init\n"); */
    dac_ctl.dma_buf_len = DAC_BUF_LEN;
    dac_ctl.cbuf_len = OUTPUT_BUF_SIZE;
    dac_ctl.dac_dma_buf = (u8 *)dac_buf;
    dac_ctl.dac_cbuf = dac_cbuf;

    memset(dac_ctl.dac_dma_buf, 0x00, sizeof(dac_buf));
    dac_buf_init(dac_ctl.dac_dma_buf, dac_ctl.dma_buf_len, dac_ctl.dac_cbuf, dac_ctl.cbuf_len);
}

DAC_CALLBACL dac_cb_fun = {
    .dvol_tab		= (u16 *)digital_vol_tab,
    .sr_cb 			= dac_sr_cb,
    .write_cb		= NULL,
    .state_cb 		= NULL,
    .delay_cb		= dac_delay_cb,
    .automute_cb 	= dac_automute_cb,
    .isr_cb			= dac_isr_cb,
};

void audio_init(void)
{
    s32 err;
    u8 bt_vol_val;
    audio_sfr_reset();
#if EQ_EN
    /* eq_init(); */
#endif

    tone_init();
    memset(&dac_ctl, 0x00, sizeof(DAC_CTL));

    get_cfg_vol_max(&(dac_ctl.sys_vol_max), &(dac_ctl.sys_vol_default));
    sys_max_vol_init(dac_ctl.sys_vol_max, dac_ctl.sys_vol_max, MAX_DIGITAL_VOL_L, MAX_DIGITAL_VOL_R);
    audio_buf_init();
    dac_callback_register(&dac_cb_fun);
#if VCOMO_EN
    dac_init_api(VCOMO_EN, LDO_SLECT, DAC_CHANNEL_SLECT, DAC_ISEL_THIRD | DAC_VSEL, VCM_RSEL_1);
#else
#if DAC_DIFF_OUTPUT
    dac_init_api(VCOMO_EN | (DAC_DIFF_OUTPUT_NO_VCM << 1), LDO_SLECT, DAC_CHANNEL_SLECT, DAC_ISEL_THIRD | DAC_VSEL, VCM_RSEL_1);
#else
    dac_init_api(VCOMO_EN, LDO_SLECT, DAC_CHANNEL_SLECT, DAC_ISEL_THIRD | DAC_VSEL, VCM_RSEL_0);
#endif
#endif

    dac_ctl.toggle = 1;

#if SYS_DEFAULT_VOL
    dac_ctl.sys_vol_l = dac_ctl.sys_vol_default;
    /* dac_ctl.sys_vol_l = SYS_DEFAULT_VOL; */
#else
    dac_ctl.sys_vol_l = bt_vol_get_memory(&dac_ctl.sys_vol_l, NV_MEM_SYS_VOL, 1);
    otp_printf("dac_ctl.sys_vol_l:%d\n", dac_ctl.sys_vol_l);
    if (dac_ctl.sys_vol_l < 10) {
        dac_ctl.sys_vol_l = 10;
    }
    if (dac_ctl.sys_vol_l > get_max_sys_vol(0)) {
        dac_ctl.sys_vol_l = get_max_sys_vol(0);
    }
    bt_vol_set_memory(&dac_ctl.sys_vol_l, NV_MEM_SYS_VOL, 1);
#endif
    dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
    dac_ctl.digital_vol_l = MAX_DIGITAL_VOL_L;
    dac_ctl.digital_vol_r = MAX_DIGITAL_VOL_R;
    sys_cur_vol_init(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, dac_ctl.digital_vol_l, dac_ctl.digital_vol_r);
    dac_set_samplerate(SR44100, 1);

    dac_channel_on(DAC_DIGITAL_CH, 1);
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, 1);
    //dac_mute(1, 0);

#if EQ_EN
    /* eq_enable(); */
#endif
}

/*----------------------------------------------------------------------------*/
/** @brief: DAC Moudule Toggle
    @param: toggle = 0,dac off
			toggle = 1,dac on
    @return:
    @author:
    @note:
*/
/*----------------------------------------------------------------------------*/
void dac_toggle(u8 toggle)
{
    if (toggle && dac_ctl.dac_off_delay) {
        /* puts("dac_off ing!!!!!!!!!!!!!!!\n"); */
        dac_ctl.dac_off_delay = 0;
        dac_ctl.toggle = 0;
    }

    if (!toggle && dac_ctl.dac_off_delay) {
        //puts("dac_offing,return\n");
        return;
    }

//   if (!toggle && bt_tone_get_status()) {
    if (!toggle && get_tone_status()) {
        //puts("bt_tone_busy,return\n");
        return;
    }


    if (toggle && !dac_ctl.toggle) {
        puts(">dac_on<\n");
        dac_ctl.toggle = 1;
        dac_on_api(VCOMO_EN, LDO_SLECT, DAC_CHANNEL_SLECT);
        dac_ie_en(1);
        set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_ON);
        dac_mute(0, 1);
    } else if (!toggle && dac_ctl.toggle) {
        puts(">dac_off<\n");
        dac_mute(1, 1);
        dac_off_pre_api();
        //dac_ctl.toggle = 0;
    }
}

void dac_delay_count(void)
{
    if (dac_ctl.dac_off_delay) {
        dac_ctl.dac_off_delay--;
        //otp_printf("dac_off_delay:%d\n",dac_ctl.dac_off_delay);
        if (dac_ctl.dac_off_delay == 0) {
            dac_ctl.toggle = 0;
            dac_off_post_api();
            dac_ie_en(0);
            puts("dac_off_delay OK\n");
        }
    }
}
void dac_automute_set(u8 ctl, u16 packet_cnt, u16 Threshold, u8 max_cnt)
{
#if DAC_AUTO_MUTE_EN
    dac_automute_init(ctl, packet_cnt, Threshold, max_cnt);
#endif
}

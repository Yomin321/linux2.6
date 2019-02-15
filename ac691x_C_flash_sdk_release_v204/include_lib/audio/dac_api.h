#ifndef __DAC_API_H__
#define __DAC_API_H__

#include "typedef.h"
#include "cpu/audio_param.h"
#include "cbuf/circular_buf.h"

//********************************************************************
//
//						DAC CONFIG
//
//********************************************************************
#define FADE_ON      1                ///<通道切换使用淡入淡出
#define FADE_OFF     0                ///<通道切换不使用淡入淡出

#define AUTO_MUTE_DISABLE	  0         ///数字通道auto mute：关
#define AUTO_MUTE_ENABLE 	 BIT(0) 	///数字通道auto mute：开
#define AUTO_UNMUTE_FADE	 BIT(1)     ///有 FADE IN 效果
#define AUTO_MUTE_UMUTE_DAC	 BIT(2)     ///Only fade out when auto mute
#define AUTO_FADE_CTL		 BIT(3)
#define AUTO_MUTE_CFG		 AUTO_MUTE_ENABLE | AUTO_UNMUTE_FADE

//********************************************************************
//
//						DAC VARIABLE
//
//********************************************************************
extern cbuffer_t dac_cb ;/*dac cbuffer*/
typedef struct _DAC_CALLBACK {
    u16  *dvol_tab;/*dac digital vol table*/
    void (*sr_cb)(u16 rate);/*dac samplerate rate callback function*/
    u32(*write_cb)(s16 *buf, u32 len);  /*dac_write callback before dac cbuf write */
    u8(*state_cb)(void);   /*get dac state:busy or idle*/
    void (*delay_cb)(u8 mode);/*dac power_on or power_off delay_callback*/
    void (*automute_cb)(u8 status);/*automute status change callback*/
    void (*isr_cb)(void *dac_buf, u8 buf_flag);
} DAC_CALLBACL;

//********************************************************************
//
//           			DAC API
//
//********************************************************************
void dac_init_api(tbool vcom_outen, u8 ldo_slect, u8 hp_type, u8 dac_isel, u8 vcm_rsel);
void dac_classD_init();
void dac_on_api(tbool vcom_outen, u8 ldo_slect, u8 hp_type);
void dac_off_pre_api();
void dac_off_post_api();
s32 ladc_isr_register(void (*ladc_isr_reg)(void));
void dac_callback_register(DAC_CALLBACL *cb);

///dac channel api
AUDIO_ERR dac_channel_on(u8 channel, u8 fade_en);
AUDIO_ERR dac_channel_off(u8 channel, u8 fade_en);
u16 get_cur_dac_channel(void);

///dac vol setting
void sys_max_vol_init(u8 max_sys_vol_l, u8 max_sys_vol_r, u8 max_digital_vol_l, u8 max_digital_vol_r);
void sys_cur_vol_init(u8 cur_sys_vol_l, u8 cur_sys_vol_r, u8 cur_digital_vol_l, u8 cur_digital_vol_r);
void set_sys_vol(u32 l_vol, u32 r_vol, u8 fade_en);
void set_digital_vol(u32 l_vol, u32 r_vol);
u8 get_sys_vol_api(u8 l_or_r);
u8 get_digital_vol_api(u8 l_or_r);
u8 get_max_sys_vol(u8 l_or_r);
u8 get_max_digital_vol(u8 l_or_r);

///dac mute api
void dac_automute_init(u8 ctl, u16 packet_cnt, u16 Threshold, u8 max_cnt);
void dac_mute(u8 mute_flag, u8 fade_en);
bool is_dac_mute(void);
bool is_auto_mute(void);

///dac data processing
void dac_digital_lr_add(void *buffer, u32 len);
void dac_digital_lr_sub(void *buffer, u32 len);
void digital_vol_tab_register(void *tab);
void dac_set_samplerate(u16 sr, u8 wait);
u16 dac_get_samplerate(void);
void digital_vol_ctrl(void *buffer, u32 len);
u32 dac_digit_energy_value(void *buffer, u16 len);

//dac buf api
void dac_buf_init(void *dma_buf, u32 dma_buf_len, void *cbuf, u32 cbuf_len);
int get_dac_cbuf_len(void);

/*----------------------------------------------------------------------------*/
/**@brief	dac_read
   @param   buf:source data
			len:source data size
   @return  rlen=0,faild
   			rlen=len,success
   @note	1)This function is called to read dac cbuffer
   			2)rlen < len:dac cbuf have not enough data to read
*/
/*----------------------------------------------------------------------------*/
int dac_read(void *buf, u32 len);

/*----------------------------------------------------------------------------*/
/**@brief	dac_write
   @param   buf:source data
			len:source data size
   @return  wlen
   @note	1)This function is called to write dac cbuffer
   			2)wlen < len:dac cbuf have not enough space to write
*/
/*----------------------------------------------------------------------------*/
u32 dac_write(s16 *buf, u32 len);

/*----------------------------------------------------------------------------*/
/**@brief	is_dac_write_able
   @param  	len
   @return  0(unable)	!0(able)
   @note	1)This function is called to check dac cbuffer space
*/
/*----------------------------------------------------------------------------*/
u32 is_dac_write_able(u32 len);

void dac_cbuf_clear(void);	/*clear cbuf*/
void dac_buf_clear(void);	/*clear dma buf*/

void dac_fade(void *ptr);
void dac_fade_ctl(u8 fade, u8 fade_speed);

void dac_digital_en(u8 digital_en, u8 trim_en);
void dac_ldo_set(u8 ldo_sel, u8 en);
void dac_ie_en(u8 en);
void dac_vout_en(u8 voutl, u8 voutr);
u32 get_dac_energy_value(void);
void audio_sfr_reset(void);
void audio_sfr_dump(void);
void dac_speaker_init();
void get_cfg_vol_max(u8 *sys_vol_max, u8 *sys_vol_default);
void dac_hp_toggle(u8 hp_type);

#endif  //__DAC_API_H__

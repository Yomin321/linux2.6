#ifndef _LADC_API_H_
#define _LADC_API_H_

#include "typedef.h"
#include "cbuf/circular_buf.h"

typedef struct _LADC_CTL {
    cbuffer_t *c_buff[4];
    u8 *buff[4];
    void (*save_ladc_buf)(void *, void *, u8, u32);
    u8  source_chl;
} LADC_CTL;
extern LADC_CTL *p_ladc;
extern LADC_CTL *p_ladc_rec;

void ladc_isr_cb_register(void (*callback_fun)(void *buf, u32 buf_flag, u32 buf_len));
void ladc_enable(u16 sr, u8 vcom_outen, u8 ch, u32 adc_cfg);
void ladc_disable(void);
void ladc_mic_en(u8 en);
void ladc_mic_gain(u8 gain, u8 gx2);
void ladc_mic_neg12(u8 en);//-12db enable
void ladc_mic_mute(u8 mute);
void ladc_sr_set(u16 sr);
u16 ladc_sr_get(void);
/*
 *********************************************************************************************************
 *
 * Description: ladc pga gain
 * Arguments  : gain = 0x0~0xF
 * Returns    : NULL
 *
 * Note       :	void ladc_pga_gain(u8 gain);
 *********************************************************************************************************
 */
void ladc_pga_gain(u8 gain);

/*
 *********************************************************************************************************
 *
 * Description:	UnDelayEnable,adc configs take effects immediately
 * Arguments  : 1=effect immediately
 *			    0=Delay effects until pending happen
 *				e.g, when you want to modify the mic_gain,you can perform it as follows:
 *					ladc_adcude(1);
 *					ladc_mic_gain(30,0);
 *					ladc_adcude(0);
 * Returns    : NULL
 *
 * Note       :	void ladc_adcude(u8 ude);
 *********************************************************************************************************
 */
void ladc_adcude(u8 ude);

#endif

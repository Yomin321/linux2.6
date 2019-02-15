/*******************************************************************************************
 File Name: dac.h

 Version: 1.00

 Discription:


 Author:yulin deng

 Email :flowingfeeze@163.com

 Date:2014-01-13 17:09:41

 Copyright:(c)JIELI  2011  @ , All Rights Reserved.
*******************************************************************************************/
#ifndef _DAC_H_
#define _DAC_H_

#include "typedef.h"

#define MAX_SYS_VOL_L         28      ///<系统主音量级数
#define MAX_SYS_VOL_R         28

#define MAX_DIGITAL_VOL_L     31      ///<系统数字音量级数
#define MAX_DIGITAL_VOL_R     31

#define LDO_SLECT  LDO_1      ///LDO选择  DAC_VDD需要电容选择LDO1，不需要电容选择LDO2 ,LDO2的性噪比差点

#define BT_CHANNEL         	DAC_DIGITAL_CH
#define MUSIC_CHANNEL      	DAC_DIGITAL_CH
#define RTC_CHANNEL        	DAC_DIGITAL_CH
#define FM_INSI_CHANNEL     DAC_DIGITAL_CH
#define FM_IIC_CHANNEL  	DAC_AMUX1
#define LINEIN_CHANNEL		DAC_AMUX1//DAC_AMUX1_L_ONLY
#define UDISK_CHANNEL      	DAC_DIGITAL_CH

typedef struct __DAC_CTL {
    u8 sys_vol_l;
    u8 sys_vol_r;
    u8 digital_vol_l;
    u8 digital_vol_r;

    u8 sys_vol_max;
    u8 sys_vol_default;

    u8 phone_vol;
    u8 *dac_dma_buf;
    u8 *dac_cbuf;
    u8 keyvoice_check;

    u8 sniff_check;
    volatile u8 toggle;
    u8 read_able;
    u8 reserved;

    volatile u32 dac_off_delay;
    u32 dma_buf_len;
    u32 cbuf_len;
} DAC_CTL;
extern DAC_CTL dac_ctl;

void audio_init(void);
void dac_toggle(u8 toggle);
void dac_check();
void dac_int_enable(void);
void dac_int_disable(void);
void dac_delay_count(void);
void dac_automute_set(u8 ctl, u16 packet_cnt, u16 Threshold, u8 max_cnt);

#endif

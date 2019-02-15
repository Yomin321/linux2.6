/*******************************************************************************************
  File Name: dac.h

Version: 1.00

Discription:


Author:yulin deng

Email :flowingfeeze@163.com

Date:2014-01-13 17:09:41

Copyright:(c)JIELI  2011  @ , All Rights Reserved.
 *******************************************************************************************/
#ifndef _EQ_H_
#define _EQ_H_

#include "typedef.h"
#define EQ_CHANNEL_NUM        2
#define EQ_SECTION_NUM        10
#define EQ_MODE_MAX           5

typedef enum {

    EQ_NORMAL = 0,
    EQ_ROCK,
    EQ_POP,
    EQ_CLASSIC,
    EQ_JAZZ,
    EQ_COUNTRY,
    EQ_CUSTOM,   //User_defined
} EQ_TYPE;

typedef struct {
    u8 mode;
    void *pEQ;
} EQ_ARG;

typedef struct {

    //u16 eq_flag;             ///<EQ FLAG :"EQ"
    u32 eq_type;             ///<EQ TYPE
    u32 eq_cnt;              ///<EQ segments
    int eq_freq_gain[7][10]; ///<EQ gain for each segments
    int global_gain[7];          ///<EQ sum gain
    int eq_filt_0[50];       ///<EQ filter 0
    int eq_filt_1[50];       ///<EQfilter 1
    int eq_filt_2[50];       ///<EQfilter 2
    int eq_filt_3[50];       ///<EQfilter 3
    int eq_filt_4[50];       ///<EQfilter 4
    int eq_filt_5[50];       ///<EQfilter 5
    int eq_filt_6[50];       ///<EQfilter 6
    int eq_filt_7[50];       ///<EQfilter 7
    int eq_filt_8[50];       ///<EQfilter 8
    int crc16;
} EQ_CFG;

void eq_init(void);
void eq_enable();
void eq_disable(void);
void eq_run(short *in, short *out, int npoint);
void eq_mode_set(u8 mode);
void eq_samplerate(u16 sr);
u8 get_eq_default_mode(void);

u8 update_ef_info(void *ef_info_addr, u16 size, u8 cnt);
s32 eq_cfg_read(void);

#endif

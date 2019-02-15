#ifndef _EQ_API_H_
#define _EQ_API_H_

#include "typedef.h"

int eq_mem_query(u8 nChannel, u8 nSection);
int eq_init_api(void *mem);
void eq_en_api(const int *coeff, u8 nChannel, u8 nSection);
void eq_dis_api(void);
void eq_run_api(short *in, short *out, int npoint);
/*
***********************************************************************************
*					 	EQ BYPASS EN
*
*Description: This function is called to switch eq calc when eq runing
*
*Argument(s): en = 1,skip eq calc
*			  en = 0,do eq calc
*
*Returns	: none
*
*Note(s)	: 1)If you don't need do eq in some situation,you call this function
***********************************************************************************
*/
void eq_bypass_en(u8 en);
/*
***********************************************************************************
*					 	EQ STATE API
*
*Description: This function is called to get current eq state
*
*Argument(s):
*
*Returns	: 0 = disable
*			 !0 = enable
*
*Note(s)	:
***********************************************************************************
*/
u8 eq_state_api(void);
int eq_mode_api(u8 mode);
int eq_samplerate_api(int sr);
int eq_config_api(int *eq_filter_44100,
                  int *eq_filter_22050,
                  int *eq_filter_11025,
                  int *eq_filter_48000,
                  int *eq_filter_24000,
                  int *eq_filter_12000,
                  int *eq_filter_32000,
                  int *eq_filter_16000,
                  int *eq_filter_8000,
                  int (*freq_gain)[10],
                  int *global_gain,
                  char en_flag);

#endif // EQ_driver_h__


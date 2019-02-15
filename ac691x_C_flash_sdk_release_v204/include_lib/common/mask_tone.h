#ifndef __MASK_TONE_H__
#define __MASK_TONE_H__
#include"typedef.h"

typedef struct __RES_DIR_T {
    u32 addr;
    u16 len;
    u16 type;	//0:warning_tone	1:sbc	2:other
} RES_DIR_T;

typedef struct __RES_INFO_T {
    u8 total;
    u8 reserve[3];
    RES_DIR_T data[0];
} __RES_INFO_T;


// tbool get_mask_tone_info(u8 tone, TONE_INFO_T *ptr);

#endif //__MASK_TONE_H__

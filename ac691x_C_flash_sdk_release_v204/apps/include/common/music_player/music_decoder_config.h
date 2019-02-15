#ifndef __MUSIC_DECODER_CONFIG_H__
#define __MUSIC_DECODER_CONFIG_H__
#include "typedef.h"
#include "dec/if_decoder_ctrl.h"
#include "sdk_cfg.h"

enum {
#if DEC_TYPE_SBC_ENABLE
    DEC_TYPE_SBC = 0,
#endif

#if DEC_TYPE_WAV_ENABLE
    DEC_TYPE_WAV,
#endif

#if DEC_TYPE_WMA_ENABLE
    DEC_TYPE_WMA,
#endif

#if DEC_TYPE_AAC_ENABLE
    DEC_TYPE_AAC,
#endif

#if DEC_TYPE_FLAC_ENABLE
    DEC_TYPE_FLAC,
#endif

#if DEC_TYPE_APE_ENABLE
    DEC_TYPE_APE,
#endif

#if DEC_TYPE_MP3_ENABLE
    DEC_TYPE_MP3,
#endif

#if DEC_TYPE_F1A_ENABLE
    DEC_TYPE_F1A,
#endif

#if DEC_TYPE_NWT_ENABLE
    DEC_TYPE_NWT,
#endif

    DEC_TYPE_NULL = 0xff,
};


#if DEC_TYPE_MP3_ENABLE
#define DEC_PHY_MP3  BIT(DEC_TYPE_MP3)
#else
#define DEC_PHY_MP3  0
#endif


#if DEC_TYPE_SBC_ENABLE
#define DEC_PHY_SBC  BIT(DEC_TYPE_SBC)
#else
#define DEC_PHY_SBC  0
#endif

#if DEC_TYPE_NWT_ENABLE
#define DEC_PHY_NWT  BIT(DEC_TYPE_NWT)
#else
#define DEC_PHY_NWT  0
#endif

#if DEC_TYPE_WAV_ENABLE
#define DEC_PHY_WAV  BIT(DEC_TYPE_WAV)
#else
#define DEC_PHY_WAV  0
#endif

#if DEC_TYPE_WMA_ENABLE
#define DEC_PHY_WMA  BIT(DEC_TYPE_WMA)
#else
#define DEC_PHY_WMA  0
#endif

#if DEC_TYPE_AAC_ENABLE
#define DEC_PHY_AAC  BIT(DEC_TYPE_AAC)
#else
#define DEC_PHY_AAC  0
#endif

#if DEC_TYPE_FLAC_ENABLE
#define DEC_PHY_FLAC BIT(DEC_TYPE_FLAC)
#else
#define DEC_PHY_FLAC 0
#endif

#if DEC_TYPE_APE_ENABLE
#define DEC_PHY_APE  BIT(DEC_TYPE_APE)
#else
#define DEC_PHY_APE 0
#endif

#if DEC_TYPE_F1A_ENABLE
#define DEC_PHY_F1A  BIT(DEC_TYPE_F1A)
#else
#define DEC_PHY_F1A 0
#endif


u32 music_decoder_get_decoder_max(void);
decoder_ops_t *music_decoder_get_index_ops(u8 index);
u8 music_decoder_get_fffr_enable(u8 index);

#endif// __MUSIC_DECODER_CONFIG_H__

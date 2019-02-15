#include "music_decoder_config.h"
#include "dec/if_decoder_ctrl.h"
#include "sdk_cfg.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".music_decoder_config_bss")
#pragma data_seg(	".music_decoder_config_data")
#pragma const_seg(	".music_decoder_config_const")
#pragma code_seg(	".music_decoder_config_code")
#endif

extern decoder_ops_t *get_mp3_ops(void);
extern decoder_ops_t *get_sbcd_ops(void);
extern decoder_ops_t *get_wav_ops(void);
extern decoder_ops_t *get_wma_ops(void);
extern decoder_ops_t *get_aac_ops(void);
extern decoder_ops_t *get_flac_ops(void);
extern decoder_ops_t *get_ape_ops(void);
extern decoder_ops_t *get_f1a_ops(void);
extern decoder_ops_t *get_nwt_ops(void);

const decoder_ops_t *(*find_dec_ops[])(void) = {
#if DEC_TYPE_SBC_ENABLE
    (void *)get_sbcd_ops,
#endif

#if DEC_TYPE_WAV_ENABLE
    (void *)get_wav_ops,
#endif

#if DEC_TYPE_WMA_ENABLE
    (void *)get_wma_ops,
#endif

#if DEC_TYPE_AAC_ENABLE
    (void *)get_aac_ops,
#endif

#if DEC_TYPE_FLAC_ENABLE
    (void *)get_flac_ops,
#endif

#if DEC_TYPE_APE_ENABLE
    (void *)get_ape_ops,
#endif

#if DEC_TYPE_F1A_ENABLE
    (void *)get_f1a_ops,
#endif

#if DEC_TYPE_MP3_ENABLE
    (void *)get_mp3_ops,
#endif

#if DEC_TYPE_NWT_ENABLE
    (void *)get_nwt_ops,
#endif
};


#define MP3_FFFR_ENABLE     1
#define SBC_FFFR_ENABLE 	0
#define WAV_FFFR_ENABLE     1
#define WMA_FFFR_ENABLE 	0
#define AAC_FFFR_ENABLE 	0
#define FLAC_FFFR_ENABLE 	0	//FLAC暂不支持快进快退
#define APE_FFFR_ENABLE 	0
#define F1A_FFFR_ENABLE 	0
#define NWT_FFFR_ENABLE 	0

const u8 dec_type_fffr_enable[] = {
#if DEC_TYPE_SBC_ENABLE
    SBC_FFFR_ENABLE,
#endif

#if DEC_TYPE_WAV_ENABLE
    WAV_FFFR_ENABLE,
#endif

#if DEC_TYPE_WMA_ENABLE
    WMA_FFFR_ENABLE,
#endif

#if DEC_TYPE_AAC_ENABLE
    AAC_FFFR_ENABLE,
#endif

#if DEC_TYPE_FLAC_ENABLE
    FLAC_FFFR_ENABLE,
#endif

#if DEC_TYPE_APE_ENABLE
    APE_FFFR_ENABLE,
#endif

#if DEC_TYPE_F1A_ENABLE
    F1A_FFFR_ENABLE,
#endif

#if DEC_TYPE_MP3_ENABLE
    MP3_FFFR_ENABLE,
#endif

#if DEC_TYPE_NWT_ENABLE
    NWT_FFFR_ENABLE,
#endif
};



u32 music_decoder_get_decoder_max(void)
{
    return (sizeof(find_dec_ops) / sizeof(find_dec_ops[0]));
}


decoder_ops_t *music_decoder_get_index_ops(u8 index)
{
    if (index >= music_decoder_get_decoder_max()) {
        return NULL;
    }
    return (decoder_ops_t *)find_dec_ops[index]();
}


u8 music_decoder_get_fffr_enable(u8 index)
{
    if (index >= music_decoder_get_decoder_max()) {
        return 0;
    }

    return dec_type_fffr_enable[index];
}

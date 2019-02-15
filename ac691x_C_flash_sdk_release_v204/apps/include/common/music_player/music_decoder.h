#ifndef __MUSIC_DECODER_H__
#define __MUSIC_DECODER_H__
#include "typedef.h"
#include "dec/if_decoder_ctrl.h"
#include "music_decoder_config.h"
// #include "sdk_cfg.h"

//enum {
//#if DEC_TYPE_SBC_ENABLE
//    DEC_TYPE_SBC = 0,
//#endif
//
//#if DEC_TYPE_WAV_ENABLE
//    DEC_TYPE_WAV,
//#endif
//
//#if DEC_TYPE_WMA_ENABLE
//    DEC_TYPE_WMA,
//#endif
//
//#if DEC_TYPE_AAC_ENABLE
//    DEC_TYPE_AAC,
//#endif
//
//#if DEC_TYPE_FLAC_ENABLE
//    DEC_TYPE_FLAC,
//#endif
//
//#if DEC_TYPE_APE_ENABLE
//    DEC_TYPE_APE,
//#endif
//
//#if DEC_TYPE_MP3_ENABLE
//    DEC_TYPE_MP3,
//#endif
//
//#if DEC_TYPE_F1A_ENABLE
//    DEC_TYPE_F1A,
//#endif
//    DEC_TYPE_NULL = 0xff,
//};
//
//
//#define DEC_PHY_MP3  BIT(DEC_TYPE_MP3)
//#define DEC_PHY_SBC  BIT(DEC_TYPE_SBC)
//
//#if DEC_TYPE_WAV_ENABLE
//#define DEC_PHY_WAV  BIT(DEC_TYPE_WAV)
//#else
//#define DEC_PHY_WAV  0
//#endif
//
//#if DEC_TYPE_WMA_ENABLE
//#define DEC_PHY_WMA  BIT(DEC_TYPE_WMA)
//#else
//#define DEC_PHY_WMA  0
//#endif
//
//#if DEC_TYPE_AAC_ENABLE
//#define DEC_PHY_AAC  BIT(DEC_TYPE_AAC)
//#else
//#define DEC_PHY_AAC  0
//#endif
//
//#if DEC_TYPE_FLAC_ENABLE
//#define DEC_PHY_FLAC BIT(DEC_TYPE_FLAC)
//#else
//#define DEC_PHY_FLAC 0
//#endif
//
//#if DEC_TYPE_APE_ENABLE
//#define DEC_PHY_APE  BIT(DEC_TYPE_APE)
//#else
//#define DEC_PHY_APE 0
//#endif
//
//#if DEC_TYPE_F1A_ENABLE
//#define DEC_PHY_F1A  BIT(DEC_TYPE_F1A)
//#else
//#define DEC_PHY_F1A 0
//#endif
//


enum {
    MAD_ERROR_FILE_END         = 0x40,
    MAD_ERROR_FILESYSTEM_ERR   = 0x41,              // NO USED
    MAD_ERROR_DISK_ERR         = 0x42,              // NO USED
    MAD_ERROR_SYNC_LIMIT       = 0x43,              // 文件错误
    MAD_ERROR_FF_FR_FILE_END   = 0x44,              //快进结束
    MAD_ERROR_FF_FR_END        = 0x45,              // NO USED
    MAD_ERROR_FF_FR_FILE_START = 0x46,              //快退到头
    MAD_ERROR_LIMIT            = 0x47,              // NO USED
    MAD_ERROR_NODATA           = 0x48,              // NO USED
};


typedef enum {
    MUSIC_DECODER_ST_STOP = 0x0,
    MUSIC_DECODER_ST_PLAY,
    MUSIC_PLAYRR_ST_PAUSE,
} MUSIC_DECODER_ST;


typedef enum {
    MUSIC_DECODER_ERR_NONE = 0x1000,
    MUSIC_DECODER_ERR_INIT_FAIL,
    MUSIC_DECODER_ERR_START_FAIL,
} MUSIC_DECODER_ERR;



typedef struct __DEC_FILE_IO {
    u32(*seek)(void *hdl, u8 type, u32 offsize);
    u32(*read)(void *hdl, u8 *buf, u16 len);
    s32(*get_size)(void *hdl, u32 *size);
} DEC_FILE_IO;



typedef struct __MUSIC_DECODER MUSIC_DECODER;


void music_decoder_loop_resume(void);

//u32 music_decoder_get_need_buf_size(void);
MUSIC_DECODER *music_decoder_creat(void);
void music_decoder_destroy(MUSIC_DECODER **hdl);
void music_decoder_set_file_interface(MUSIC_DECODER *obj, DEC_FILE_IO *_io, void *hdl);
void music_decoder_set_output_interface(MUSIC_DECODER *obj, int (*cbk)(void *priv, void *buf, u32 len), void *priv);
void music_decoder_set_setInfo_interface(MUSIC_DECODER *obj, u32(*cbk)(void *priv, dec_inf_t *inf, tbool wait), void *priv);
void music_decoder_set_data_clear_interface(MUSIC_DECODER *obj, void (*cbk)(void *priv), void *priv);
void music_decoder_set_data_wait_interface(MUSIC_DECODER *obj, tbool(*cbk)(void *priv), void *priv);
void music_decoder_set_err_deal_interface(MUSIC_DECODER *obj, void (*cbk)(void *priv, u32 err), void *priv);
void music_decoder_set_ini_sta(MUSIC_DECODER *obj, MUSIC_DECODER_ST sta);
#if 0
void music_decoder_set_father_name(MUSIC_DECODER *obj, const char *name);
void music_decoder_set_my_name(MUSIC_DECODER *obj, const char *name);
#endif
void music_decoder_set_spec_format_name(MUSIC_DECODER *obj, const char *name);
void music_decoder_set_decoder_type(MUSIC_DECODER *obj, u32 param);
void music_decoder_set_dev(MUSIC_DECODER *obj, u32 dev);
void music_decoder_set_AAC_decoder_defualt_info(MUSIC_DECODER *obj, u32 sr, u32 nch, u32 br);
#if 0
void *music_decoder_get_father_name(MUSIC_DECODER *obj);
void *music_decoder_get_my_name(MUSIC_DECODER *obj);
#endif
u32 music_decoder_get_decode_total_time(MUSIC_DECODER *obj);
u32 music_decoder_get_decode_cur_time(MUSIC_DECODER *obj);
u32 music_decoder_get_break_point_size(MUSIC_DECODER *obj);
u8 *music_decoder_get_break_point_info(MUSIC_DECODER *obj);
MUSIC_DECODER_ST music_decoder_get_status(MUSIC_DECODER *obj);
u32 music_decoder_play(MUSIC_DECODER *obj);
void music_decoder_decode_deal(MUSIC_DECODER *obj);
void music_decoder_stop(MUSIC_DECODER *obj);
MUSIC_DECODER_ST music_decoder_pp(MUSIC_DECODER *obj);
void music_decoder_fffr(MUSIC_DECODER *obj, u8 type, u8 second);
void music_decoder_set_break_point(MUSIC_DECODER *obj, void *buf, u32 len);

#endif//__MUSIC_DECODER_H__


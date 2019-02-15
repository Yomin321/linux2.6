#include "music_decoder.h"
#include "strings.h"
#include "sdk_cfg.h"
#include "irq_api.h"
#include "common.h"
#include "uart.h"

//#define MUSIC_DECODER_DEBUG_ENABLE

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".music_decoder_bss")
#pragma data_seg(	".music_decoder_data")
#pragma const_seg(	".music_decoder_const")
#pragma code_seg(	".music_decoder_code")
#endif

#ifdef MUSIC_DECODER_DEBUG_ENABLE
#define music_decoder_printf	otp_printf
#else
#define music_decoder_printf(...)
#endif

#define MUSIC_DECODER_WITHOUT_OS

/* #define MUSIC_DECODER_MALLOC_EN */
#ifdef  MUSIC_DECODER_MALLOC_EN
#include "mem/malloc.h"
#define music_decoder_malloc	malloc
#define music_decoder_calloc	calloc
#define music_decoder_free		free
#else
#define music_decoder_malloc(...)
#define music_decoder_calloc(...)
#define music_decoder_free(...)
#endif//MUSIC_DECODER_MALLOC_EN


#define UPCASE(a)	\
	(((a)>='a' && (a)<='z') ? (a)-('a'-'A'):(a))



typedef enum __FORMAT_STATUS {
    FORMAT_OK = 0,
    FORMAT_OK_BUT_NO_SUPPORT,                 //格式正确但不支持的时候，不再继续检查
    FORMAT_ERR,
    FORMAT_NOT_DETECTED,
} _FORMAT_STATUS;



typedef struct __FFFR {
    u32 			output_cnt;;
    u8 				enable;
    volatile u8 	type;
} FFFR;



typedef union {
    AAC_DEFAULT aac;
} DEFAULT_SETTING;


typedef struct __DEC_FILE {
    void 					*hdl;
    DEC_FILE_IO 			*_io;
} DEC_FILE;

typedef struct __DECODER_OPS {
    void 					*hdl;
    audio_decoder_ops		*ops;
} DECODER_OPS;


typedef struct __OUTPUT_IO {
    void *priv;
    int (*cbk)(void *priv, void *buf, u32 len);
} OUTPUT_IO;

typedef struct __SET_INFO_IO {
    void *priv;
    u32(*cbk)(void *priv, dec_inf_t *inf, tbool wait);
} SET_INFO_IO;

typedef struct __OUTPUT_WAIT_IO {
    void *priv;
    tbool(*cbk)(void *priv);
} OUTPUT_WAIT_IO;

typedef struct __OUTPUT_CLEAR_IO {
    void *priv;
    void (*cbk)(void *priv);
} OUTPUT_CLEAR_IO;

typedef struct __DATA_STREAM_IO {
    OUTPUT_IO 			output;
    OUTPUT_WAIT_IO		wait;
    OUTPUT_CLEAR_IO		clear;
    SET_INFO_IO			set_info;
} DATA_STREAM_IO;

typedef struct __ERR_DEAL_IO {
    void *priv;
    void (*cbk)(void *priv, u32 err);
} ERR_DEAL_IO;

typedef struct __BREAK_POINT {
    void						*bp_buf;
    u32						 	bp_size;
} BREAK_POINT;


struct __MUSIC_DECODER {
    DECODER_OPS				 	dec_ops;
    struct if_decoder_io	 	dec_io;
    DATA_STREAM_IO			 	stream_io;
    ERR_DEAL_IO					err_io;
    DEC_FILE 				 	file;
    DEFAULT_SETTING			 	default_seting;
    FFFR					 	ff_fr;
    BREAK_POINT					bp_info;

#if 0
    const char 					*father_name;
    const char 					*my_name;
#endif
    const char 					*format_name;
    volatile MUSIC_DECODER_ST  	status;
    u32 					 	type_enable;
    u8							read_err;
    u8							resume;
    MUSIC_DECODER_ST            ini_sta;
} ;

MUSIC_DECODER music_decoder_hdl;
u32 music_decode_ram[5 * 1024] 	sec_used(.music_mp3_dec);
u32 sbc_decode_ram[3 * 1024] 	sec_used(.bt_sbc_dec);



static tbool music_decoder_decoder_match(const char *dec_name, const char *format_name)
{
    int i;
    if (*format_name == '\0') {
        return false;
    }
    for (i = 0; i < strlen(dec_name); i++) {
        if (UPCASE(dec_name[i]) != UPCASE(format_name[i])) {
            break;
        }
    }
    if (i == strlen(dec_name)) {
        return true;
    }

    if (strlen(dec_name) >=  strlen(format_name)) {
        return false;
    }

    return music_decoder_decoder_match(dec_name, format_name + strlen(dec_name));
}

static s32 music_decoder_decode_check_buf(void *priv, u32 faddr, u8 *buf)
{
    s32 read_len = 0;
    MUSIC_DECODER *dec = (MUSIC_DECODER *)priv;
    DEC_FILE *file;
    if (dec == NULL || dec->file._io == NULL) {
        return 0;
    }
    file = (DEC_FILE *) & (dec->file);
    if (file->_io->seek) {
        file->_io->seek(file->hdl, SEEK_SET, faddr);
    }
    if (file->_io->read) {
        read_len = file->_io->read(file->hdl, buf, 512);
    }

    if (read_len == (u16) - 1) {
        music_decoder_printf("\nfun = %s, line = %d\n", __func__, __LINE__);
        dec->read_err = 1;
        return 0;
    }
    return read_len;
}


static s32 music_decoder_decode_input(void *priv, u32 faddr, u8 *buf, u32 len, u8 type)
{
    MUSIC_DECODER *dec = (MUSIC_DECODER *)priv;
    DEC_FILE *file;
    if (dec == NULL || dec->file._io == NULL) {
        return 0;
    }
    file = (DEC_FILE *) & (dec->file);
    u16 read_len = 0;
    if (type == 0) {
        if (file->_io->seek) {
            file->_io->seek(file->hdl, SEEK_SET, faddr);
        }
        if (file->_io->read) {
            read_len = file->_io->read(file->hdl, buf, len);
        }
    }

    if ((u16) - 1 == read_len) {
        music_decoder_printf("\nfun = %s, line = %d\n", __func__, __LINE__);
        dec->read_err = 1;
        return 0;
    }

    return read_len;
}

static int music_decoder_decode_output(void *priv, void *buf, u32 len)
{
    MUSIC_DECODER *dec = (MUSIC_DECODER *)priv;

    int out_len = 0;
    if (dec) {
        OUTPUT_IO *output = (OUTPUT_IO *) & (dec->stream_io.output);

        if (dec->ff_fr.output_cnt > len) {
            dec->ff_fr.output_cnt -= len;
        } else {
            dec->ff_fr.output_cnt = 0;
        }

        if (output->cbk) {
            out_len = output->cbk(output->priv, buf, len);
        }
    }

    return out_len;
}

static u32 music_decoder_decode_store_rev_data(void *priv, u32 addr, int len)
{
    return len;
}

static u32 music_decoder_decode_get_lslen(void *priv)
{
    MUSIC_DECODER *dec = (MUSIC_DECODER *)priv;
    DEC_FILE *file;
    if (dec == NULL || dec->file._io == NULL) {
        return 0;
    }
    file = (DEC_FILE *) & (dec->file);
    if (file->_io->get_size) {
        u32 size = 0;
        if (file->_io->get_size(file->hdl, &size)) {
            return (u32) - 1;
        } else {
            music_decoder_printf("\n file size = %x ,fun = %s, line = %d\n", size, __func__, __LINE__);
            return size;
        }
    }

    return (u32) - 1;
}

SET_INTERRUPT
void music_decoder_loop()
{
    irq_common_handler(IRQ_SOFT_IDX);
    MUSIC_DECODER *obj = &music_decoder_hdl;
    music_decoder_decode_deal(obj);
}

void music_decoder_loop_resume(void)
{
    irq_set_pending(IRQ_SOFT_IDX);
}



MUSIC_DECODER *music_decoder_creat(void)
{
    MUSIC_DECODER *obj;

#ifdef  MUSIC_DECODER_MALLOC_EN
    u8 *need_buf;
    u32 need_buf_len = 0;
    need_buf_len = SIZEOF_ALIN(sizeof(MUSIC_DECODER), 4);
    need_buf = (u8 *)music_decoder_calloc(1, need_buf_len);
    if (need_buf == NULL) {
        return NULL;
    }
#else
    memset((u8 *)&music_decoder_hdl, 0x0, sizeof(MUSIC_DECODER));
    obj = (MUSIC_DECODER *)&music_decoder_hdl;
#endif

    obj->ini_sta = MUSIC_DECODER_ST_PLAY;
    return obj;
}

void music_decoder_destroy(MUSIC_DECODER **hdl)
{
    if ((hdl == NULL) || (*hdl == NULL)) {
        return ;
    }
    MUSIC_DECODER *obj = *hdl;
    music_decoder_stop(obj);

#ifdef  MUSIC_DECODER_MALLOC_EN
    music_decoder_free(*hdl);
#endif
    *hdl = NULL;
}

void music_decoder_set_file_interface(MUSIC_DECODER *obj, DEC_FILE_IO *_io, void *hdl)
{
    if (obj == NULL) {
        return ;
    }
    obj->file.hdl = hdl;
    obj->file._io = _io;
}


void music_decoder_set_output_interface(MUSIC_DECODER *obj, int (*cbk)(void *priv, void *buf, u32 len), void *priv)
{
    if (obj == NULL) {
        return ;
    }
    obj->stream_io.output.priv = priv;
    obj->stream_io.output.cbk = cbk;
}

void music_decoder_set_setInfo_interface(MUSIC_DECODER *obj, u32(*cbk)(void *priv, dec_inf_t *inf, tbool wait), void *priv)
{
    if (obj == NULL) {
        return ;
    }
    obj->stream_io.set_info.priv = priv;
    obj->stream_io.set_info.cbk = cbk;
}

void music_decoder_set_data_clear_interface(MUSIC_DECODER *obj, void (*cbk)(void *priv), void *priv)
{
    if (obj == NULL) {
        return ;
    }
    obj->stream_io.clear.priv = priv;
    obj->stream_io.clear.cbk = cbk;
}

void music_decoder_set_data_wait_interface(MUSIC_DECODER *obj, tbool(*cbk)(void *priv), void *priv)
{
    if (obj == NULL) {
        return ;
    }
    obj->stream_io.wait.priv = priv;
    obj->stream_io.wait.cbk = cbk;
}

void music_decoder_set_err_deal_interface(MUSIC_DECODER *obj, void (*cbk)(void *priv, u32 err), void *priv)
{
    if (obj == NULL) {
        return ;
    }
    obj->err_io.priv = priv;
    obj->err_io.cbk = cbk;
}


void music_decoder_set_spec_format_name(MUSIC_DECODER *obj, const char *name)
{
    if (obj == NULL) {
        return ;
    }
    obj->format_name = name;
}

void music_decoder_set_decoder_type(MUSIC_DECODER *obj, u32 param)
{
    if (obj == NULL) {
        return ;
    }
    obj->type_enable |= param;
}


void music_decoder_set_AAC_decoder_defualt_info(MUSIC_DECODER *obj, u32 sr, u32 nch, u32 br)
{
    if (obj == NULL) {
        return ;
    }
    AAC_DEFAULT *aac_defult = (AAC_DEFAULT *) & (obj->default_seting);
    aac_defult->samplerate = sr;
    aac_defult->nchannels = nch;
    aac_defult->bitrate = br;
}

void music_decoder_set_break_point(MUSIC_DECODER *obj, void *buf, u32 len)
{
    if (obj == NULL) {
        return ;
    }
    obj->bp_info.bp_buf = buf;
    obj->bp_info.bp_size = len;
}

void music_decoder_set_ini_sta(MUSIC_DECODER *obj, MUSIC_DECODER_ST sta)
{
    if (obj == NULL) {
        return ;
    }
    obj->ini_sta = sta;
}

u32 music_decoder_get_decode_total_time(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return 0;
    }

    DECODER_OPS *dec_ops = &(obj->dec_ops);
    if (dec_ops->ops != NULL && dec_ops->hdl != NULL) {
        if (dec_ops->ops->get_dec_inf) {
            dec_inf_t *dec_inf = dec_ops->ops->get_dec_inf(dec_ops->hdl);
            if (dec_inf) {
                return dec_inf->total_time;
            }
        }
    }
    return 0;
}

u32 music_decoder_get_decode_cur_time(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return 0;
    }

    DECODER_OPS *dec_ops = &(obj->dec_ops);
    if (dec_ops->ops != NULL && dec_ops->hdl != NULL) {
        if (dec_ops->ops->get_playtime) {
            return dec_ops->ops->get_playtime(dec_ops->hdl);
        }
    }

    return 0;
}


MUSIC_DECODER_ST music_decoder_get_status(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return MUSIC_DECODER_ST_STOP;
    }
    return obj->status;
}


u32 music_decoder_get_break_point_size(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return 0;
    }
    DECODER_OPS *cur_ops = &(obj->dec_ops);
    if (cur_ops->hdl && cur_ops->ops && cur_ops->ops->need_bpbuf_size) {
        return cur_ops->ops->need_bpbuf_size(cur_ops->hdl);
    }
    return 0;
}

u8 *music_decoder_get_break_point_info(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return NULL;
    }

    DECODER_OPS *cur_ops = &(obj->dec_ops);
    if (cur_ops->hdl && cur_ops->ops && cur_ops->ops->get_bp_inf) {
        return (u8 *)cur_ops->ops->get_bp_inf(cur_ops->hdl);
    } else {
        return NULL;
    }
}


u32 music_decoder_play(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return MUSIC_DECODER_ERR_INIT_FAIL;
    }

    u32 i;
    u32 dec_need_buf_size;
    _FORMAT_STATUS fm_err = FORMAT_ERR;
    /* decoder_ops_t *dec_ops = NULL; */
    DATA_STREAM_IO *stream_io = &(obj->stream_io);
    struct if_decoder_io *dec_io = &(obj->dec_io);

    dec_io->priv = (void *)obj;
    dec_io->input = (void *)music_decoder_decode_input;
    dec_io->check_buf = (void *)music_decoder_decode_check_buf;
    dec_io->output = (void *)music_decoder_decode_output;
    dec_io->get_lslen = (void *)music_decoder_decode_get_lslen;
    dec_io->store_rev_data = (void *)music_decoder_decode_store_rev_data;

    DECODER_OPS *dec_ops = &(obj->dec_ops);

    for (i = 0; i < music_decoder_get_decoder_max(); i++) {
        dec_ops->ops =  music_decoder_get_index_ops(i);
        if (dec_ops->ops == NULL) {
            continue;
        }
        if (obj->format_name != NULL) {
            if (music_decoder_decoder_match(dec_ops->ops->name, obj->format_name) == false) {
                continue ;
            }
        } else if (0 == (obj->type_enable & BIT(i))) {
            continue ;
        }


        if (obj->file._io != NULL && obj->file._io->seek != NULL) {
            obj->file._io->seek(obj->file.hdl, SEEK_SET, 0);
        }

        dec_need_buf_size = dec_ops->ops->need_dcbuf_size();

        music_decoder_printf("dec need buf size %d\n", dec_need_buf_size);
#ifdef  MUSIC_DECODER_MALLOC_EN
        dec_ops->hdl = (void *)music_decoder_calloc(1, dec_need_buf_size);
#else
        if (obj->type_enable & (DEC_PHY_SBC | DEC_PHY_NWT)) {
            music_decoder_printf("dec play sbc or nwt\n");
            memset(sbc_decode_ram, 0x00, sizeof(sbc_decode_ram));
            dec_ops->hdl = (void *)sbc_decode_ram;
        } else {
            memset(music_decode_ram, 0x00, sizeof(music_decode_ram));
            dec_ops->hdl = (void *)music_decode_ram;
        }
#endif
        if (dec_ops->hdl == NULL) {
            music_decoder_printf("malloc fail!! fun = %s, line = %d\n", __func__, __LINE__);
            continue ;
        }

        if (dec_ops->ops->need_rdbuf_size) {
            music_decoder_printf("need_rdbuf_size = %d, fun = %s, line = %d\n", dec_ops->ops->need_rdbuf_size(), __func__, __LINE__);
        }

#if 0		//断点长度获取，SBC 和 WT 暂时不适用
        if (dec_ops->ops->need_bpbuf_size) {
            obj->bp_info.bp_size = dec_ops->ops->need_bpbuf_size(dec_ops->hdl);
        }
#endif
///bp enable
        {
            music_decoder_printf("obj->bp_info.bp_buf = 0x%x\n", obj->bp_info.bp_buf);
            dec_ops->ops->open(dec_ops->hdl, (const struct if_decoder_io *)dec_io, obj->bp_info.bp_buf);

        }

        if (dec_ops->ops->format_check) {
            fm_err = dec_ops->ops->format_check(dec_ops->hdl);
        } else {
            if ((0 == strcmp("AAC", obj->format_name))
                || (0 == strcmp("SBC", obj->format_name))
                || (0 == strcmp("WT", obj->format_name))) {
                fm_err = FORMAT_OK;
            }
            break;
        }

        if ((fm_err) && (fm_err != FORMAT_OK_BUT_NO_SUPPORT)) {
#ifdef  MUSIC_DECODER_MALLOC_EN
            music_decoder_free(dec_ops->hdl);
#endif
            dec_ops->hdl = NULL;
            continue ;
        }

        ///fffr enable
        obj->ff_fr.enable = music_decoder_get_fffr_enable(i);

        break;
    }

    /* music_decoder_printf("fun = %s, line = %d\n", __func__, __LINE__);		 */
    if (FORMAT_OK != fm_err) {
#ifdef  MUSIC_DECODER_MALLOC_EN
        music_decoder_free(dec_ops->hdl);
#endif
        dec_ops->hdl = NULL;
        return MUSIC_DECODER_ERR_START_FAIL;
    }

    ///get id3
    //

    /* music_decoder_printf("fun = %s, line = %d\n", __func__, __LINE__);		 */
    if (dec_ops->ops->dec_confing) {
        ///choose output data control
        AUDIO_DECODE_PARA dec_mode_obj;
        dec_mode_obj.mode = 1;/*1 need return writeout len when dec output, 0 not*/
        dec_ops->ops->dec_confing(dec_ops->hdl, SET_DECODE_MODE, (void *)&dec_mode_obj);

        if (0 == strcmp("AAC", obj->format_name)) {
            dec_ops->ops->dec_confing(dec_ops->hdl, SET_AAC_SR_NCH_BR,  &(obj->default_seting));
        } else {

        }
    }

    if (stream_io->clear.cbk) {
        stream_io->clear.cbk(stream_io->clear.priv);
    }

    /* if (dec_ops->ops->format_check)  */
    {
        /* if((0 != strcmp("SBC", obj->format_name))) */
        {
            if (stream_io->set_info.cbk) {
                stream_io->set_info.cbk(stream_io->set_info.priv, dec_ops->ops->get_dec_inf(dec_ops->hdl), 1);
            }

        }

    }

    obj->status = obj->ini_sta;
    if (obj->ini_sta != MUSIC_DECODER_ST_PLAY) {
        obj->ini_sta = MUSIC_DECODER_ST_PLAY;
    }

    music_decoder_printf("music player play ok!!\n");
    irq_handler_register(IRQ_SOFT_IDX, music_decoder_loop, irq_index_to_prio(IRQ_SOFT_IDX));
    return MUSIC_DECODER_ERR_NONE;
}


void music_decoder_decode_deal(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return ;
    }

    u32 res = 0;
    DECODER_OPS *dec_ops = &(obj->dec_ops);
    DATA_STREAM_IO *stream_io = &(obj->stream_io);
    if (dec_ops->hdl == NULL) {
        return ;
    }

    if (obj->status == MUSIC_DECODER_ST_STOP) {
        return ;
    }

    if (obj->status == MUSIC_DECODER_ST_PLAY) {
        u8 fffr = PLAY_MOD_NORMAL;
        if (obj->ff_fr.enable) {
            fffr = obj->ff_fr.type;
            obj->ff_fr.type = PLAY_MOD_NORMAL;
            if (fffr > PLAY_MOD_FB) {
                fffr = PLAY_MOD_NORMAL;
            }
        }

        res = dec_ops->ops->run(dec_ops->hdl, fffr);

        if (res) {
            if (music_decoder_decoder_match(dec_ops->ops->name, "AACSBC") == true) {
                if (res == 99) {

                    dec_ops->ops->dec_confing(dec_ops->hdl, CLEAR_DECODE_STATE,  NULL);
                    music_decoder_printf("AACSBC err!!\n");
                }
                res = 0;
            } else {
                if (obj->read_err == 0) {
                    if (res == MAD_ERROR_NODATA) {
                        res = MAD_ERROR_FILE_END;
                    }
                }
            }
        } else {
            if (music_decoder_decoder_match(dec_ops->ops->name, "MP3AACSBC") == true) {
                if (dec_ops->ops->get_dec_inf) {
                    dec_inf_t *dec_inf = dec_ops->ops->get_dec_inf(dec_ops->hdl);
                    if (stream_io->set_info.cbk) {
                        stream_io->set_info.cbk(stream_io->set_info.priv, dec_ops->ops->get_dec_inf(dec_ops->hdl), 1);
                    }
                }
            }

        }

        if (obj->err_io.cbk) {
            obj->err_io.cbk(obj->err_io.priv, res);
        }

    }


    if (res) {
        obj->status = MUSIC_DECODER_ST_STOP;
    }
}


void music_decoder_stop(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return ;
    }
    if (obj->status != MUSIC_DECODER_ST_STOP) {
#ifdef MUSIC_DECODER_WITHOUT_OS
        obj->status = MUSIC_DECODER_ST_STOP;
#else

#endif

    }
    music_decoder_printf("-----------------music_decoder_stop-----------------------\n");


    irq_handler_unregister(IRQ_SOFT_IDX);

    DECODER_OPS *dec_ops = &(obj->dec_ops);
#ifdef  MUSIC_DECODER_MALLOC_EN
    music_decoder_free(dec_ops->hdl);
#endif
    dec_ops->hdl = NULL;
    //clear all last play status
}

MUSIC_DECODER_ST music_decoder_pp(MUSIC_DECODER *obj)
{
    if (obj == NULL) {
        return MUSIC_DECODER_ST_STOP;
    }
    MUSIC_DECODER_ST wait_st = MUSIC_DECODER_ST_STOP;
#ifdef MUSIC_DECODER_WITHOUT_OS
    if (obj->status == MUSIC_DECODER_ST_PLAY) {
        obj->status = MUSIC_PLAYRR_ST_PAUSE;
    } else {
        obj->status = MUSIC_DECODER_ST_PLAY;
    }
#else

#endif
    return obj->status;
}

void music_decoder_fffr(MUSIC_DECODER *obj, u8 type, u8 second)
{
    if (obj == NULL) {
        return ;
    }

    if (obj->ff_fr.enable == 0) {
        return;
    }

    if (type > PLAY_MOD_FB) {
        return;
    }

    if (obj->ff_fr.output_cnt == 0) {
        DECODER_OPS *dec_ops = &(obj->dec_ops);
        obj->ff_fr.type = type;
        if (dec_ops->ops->set_step) {
            dec_ops->ops->set_step(dec_ops->hdl, second);
        }
        if (dec_ops->ops->get_dec_inf) {
            dec_inf_t *dec_inf = dec_ops->ops->get_dec_inf(dec_ops->hdl);
            obj->ff_fr.output_cnt = ((dec_inf->sr * 4) / 10) * dec_inf->nch;
        }
    }
}




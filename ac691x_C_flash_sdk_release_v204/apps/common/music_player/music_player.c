#include "music_player.h"
#include "music_decoder.h"
#include "file_operate.h"
#include "dev_mg_api.h"
#include "file_op_err.h"
#include "fs_io.h"
#include "audio/dac_api.h"
#include "msg.h"
#include "task_manager.h"
#include "file_io.h"
#include "dev_manage.h"
#include "flash_api.h"
#include "dec/warning_tone.h"
#include "audio/dac.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".music_decoder_bss")
#pragma data_seg(	".music_decoder_data")
#pragma const_seg(	".music_decoder_const")
#pragma code_seg(	".music_decoder_code")
#endif

/* #define MUSIC_PLAYER_DEBUG_ENABLE */

#ifdef MUSIC_PLAYER_DEBUG_ENABLE
#define music_player_printf	    otp_printf
#define music_player_puts       puts
#else
#define music_player_printf(...)
#define music_player_puts(...)
#endif

/* #define MUSIC_PLAYER_MALLOC_EN */
#ifdef  MUSIC_PLAYER_MALLOC_EN
#include "mem/malloc.h"
#define music_player_malloc	malloc
#define music_player_calloc	calloc
#define music_player_free	free
#endif

struct __MUSIC_PLAYER {
    MUSIC_DECODER *dop;
    FILE_OPERATE *fop;

};
void set_sbc_energy_detect_flag(u8 flag);
static u32 fs_get_file_size(void *priv)
{
    u32 size;
    if (fs_io_ctrl(NULL, (_FIL_HDL *)priv, FS_IO_GET_FILE_SIZE, &size)) {
        return 0;
    }
    // printf("===================file size === %d\n", size);
    return size;
}
static MUSIC_PLAYER music_player_hdl sec_used(.music_mem);

static const DEC_FILE_IO music_player_file = {
    .seek = (void *)fs_seek,
    .read = (void *)fs_read,
    .get_size = (void *)fs_get_file_size,
};



MUSIC_DECODER *music_player_get_dop(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return NULL;
    }

    return obj->dop;
}

FILE_OPERATE *music_player_get_fop(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return NULL;
    }

    return obj->fop;
}



void music_player_run(MUSIC_PLAYER *obj)
{
    music_decoder_decode_deal(obj->dop);
}

static u32 music_player_set_info(void *priv, dec_inf_t *inf, tbool wait)
{
    dac_set_samplerate(inf->sr, wait);
    return 0;
}

static int music_player_output(void *priv, void *buf, u32 len)
{
#if 1
    int out_len = (int)dac_write((s16 *)buf, len);
    if (out_len != ((int)len)) {
    } else {
        music_decoder_loop_resume();   /* _thread_reset_resume_byname(MUSIC_PLAYER_TASK, 1); */
    }
#endif
    return out_len;
}


static tbool music_player_get_vm_index(MUSIC_PLAYER *obj, u8 *index)
{
    if (obj == NULL) {
        return false;
    }
    DEV_HANDLE dev = file_operate_get_dev(obj->fop);
    if (dev == sd0) {
        *index = VM_DEV0_BREAKPOINT;
    } else if (dev == sd1) {
        *index = VM_DEV1_BREAKPOINT;
    } else if (dev == usb) {
        *index = VM_DEV2_BREAKPOINT;
    } else {
        return false;
    }
    return true;
}


static tbool music_player_get_break_point_info(MUSIC_PLAYER *obj, MUSIC_PLAYER_BP *bp_info)
{
    if (obj == NULL || bp_info == NULL) {
        return false;
    }

//    if (music_decoder_get_status(obj->dop) != MUSIC_DECODER_ST_STOP) {
    if (obj->dop) {
        memset((u8 *)bp_info, 0, sizeof(MUSIC_PLAYER_BP));

        u32 bp_size = music_decoder_get_break_point_size(obj->dop);
        if ((bp_size > sizeof(bp_info->buf)) || (bp_size == 0)) {
            music_player_printf("break point buf is too small!! bp_size = %d\n\n", bp_size);
            return false;
        } else {
            u8 *bp_buf = music_decoder_get_break_point_info(obj->dop);
            memcpy(bp_info->buf, bp_buf, bp_size);
            bp_info->bp_size = bp_size;

            //printf("\n\n\n------get_break point buf bp_size = %d\n\n", bp_size);
            //printf_buf(bp_buf, bp_info->bp_size);

        }

        if (fs_io_ctrl(NULL, (_FIL_HDL *)file_operate_get_file_hdl(obj->fop), FS_IO_GET_FILE_SIZE, &(bp_info->f_info.f_size))) {
            return false;
        }
        if (fs_io_ctrl(NULL, (_FIL_HDL *)file_operate_get_file_hdl(obj->fop), FS_IO_GET_FILE_SCLUST, &(bp_info->f_info.sclust))) {
            return false;
        }

        ///可以根据不同设备保存断点到不同的存储空间
        /* bp_info->dev = file_operate_get_dev(obj->fop);	 */
        return true;
    }
    return false;
}



static void music_player_save_break_point(MUSIC_PLAYER *obj)
{
    MUSIC_PLAYER_BP bp_info;

    if (obj == NULL) {
        return ;
    }

    if (music_player_get_break_point_info(obj, &bp_info) == false) {
        return ;
    }
    printf("fun %s\nsclust = %d\nf_size = %d\n", __func__, bp_info.f_info.sclust, bp_info.f_info.f_size);

    u8 vm_index = 0;
    if (music_player_get_vm_index(obj, &vm_index) == false) {
        return ;
    }

    s32 ret;
    u32 bp_size = bp_info.bp_size + sizeof(bp_info) - sizeof(bp_info.buf);
    ret = vm_write(vm_index, (const void *)&bp_info, bp_size);
    if (0 < ret) {
        printf("save bp ok , len = %d\n", ret);
        /* printf_buf((u8 *)&bp_info, ret); */
    } else {
        printf("warnning : save bp err %d\n", ret);
    }
}

static tbool music_player_read_break_point(MUSIC_PLAYER *obj, MUSIC_PLAYER_BP *bp_info)
{
    if (obj == NULL || bp_info == NULL) {
        return false;
    }

    if (file_operate_get_file_sel_mode(obj->fop) != PLAY_BREAK_POINT) {
        return false;
    }

    memset((u8 *)bp_info, 0, sizeof(MUSIC_PLAYER_BP));

    u8 vm_index = 0;
    if (music_player_get_vm_index(obj, &vm_index) == false) {
        return false;
    }

    s32 ret;
    u32 bp_size = sizeof(MUSIC_PLAYER_BP);		//read max break_point len
    ret = vm_read(vm_index, (void *)bp_info, bp_size);
    if (0 < ret) {
        printf("read bp ok!!! , len = %d\n", ret);
    } else {
        printf("warnning : read bp err\n");
        return false;
    }

    /* printf_buf((u8 *)bp_info, ret); */
    return true;
}

static void music_player_clear_break_point(MUSIC_PLAYER *obj, u32 err)
{
    if (obj == NULL) {
        return ;
    }

    if ((err == MUSIC_DECODER_ERR_NONE) || (err == FILE_OP_ERR_END_FILE)) { //解码成功和设备最后一个文件才需要清除断点
        MUSIC_PLAYER_BP bp_info;
        ///只是记录文件簇号和文件大小
        memset((u8 *)&bp_info, 0, sizeof(MUSIC_PLAYER_BP));
        if (err == MUSIC_DECODER_ERR_NONE) {
            if (fs_io_ctrl(NULL, (_FIL_HDL *)file_operate_get_file_hdl(obj->fop), FS_IO_GET_FILE_SIZE, &(bp_info.f_info.f_size))) {
                return ;
            }
            if (fs_io_ctrl(NULL, (_FIL_HDL *)file_operate_get_file_hdl(obj->fop), FS_IO_GET_FILE_SCLUST, &(bp_info.f_info.sclust))) {
                return ;
            }
        }

        ///可以根据不同设备保存断点到不同的存储空间
        u8 vm_index = 0;
        if (music_player_get_vm_index(obj, &vm_index) == false) {
            return ;
        }

        s32 ret;
        u32 bp_size = sizeof(bp_info) - sizeof(bp_info.buf);
        ret = vm_write(vm_index, (const void *)&bp_info, bp_size);
        if (0 < ret) {
            printf("clear bp ok, len = %d\n", ret);
        } else {
            printf("warnning : clear bp err\n");
        }
    }
}


static void music_player_decoder_err_deal(void *priv, u32 err)
{
    u32 cur_dev_status = 1;
    u32 msg;
    s32 ret = 0;
    MUSIC_PLAYER *obj = (MUSIC_PLAYER *)priv;

    if (err) {
        ret = dev_get_online_status((DEV_HANDLE)file_operate_get_dev(obj->fop), &cur_dev_status);
        if (ret != 0) {
            msg = SYS_EVENT_DEC_DEVICE_ERR;
        } else {
            if (cur_dev_status == 0) {
                msg = SYS_EVENT_DEC_DEVICE_ERR;
            } else {
                printf("music player dec err = %x\n", err);
                switch (err) {
                case MAD_ERROR_FILE_END://file end
                    msg = SYS_EVENT_DEC_END;
                    break;
                case MAD_ERROR_SYNC_LIMIT:// 文件错误 */
                    msg = SYS_EVENT_DEC_END;
                    break;
                case MAD_ERROR_FF_FR_FILE_END://快进结束 */
                    msg = SYS_EVENT_DEC_FF_END;
                    break;
                case MAD_ERROR_FF_FR_FILE_START://快退结束 */
                    msg = SYS_EVENT_DEC_FR_END;
                    break;
                case MAD_ERROR_NODATA:
                    msg = SYS_EVENT_DEC_END;
                    break;
                default:
                    msg = SYS_EVENT_DEC_END;
                    break;
                }
            }
        }
        /* music_player_printf("music player dec err = %x, father name = %s\n", err, (const char *)music_decoder_get_father_name(obj->dop)); */
        task_post_msg(NULL, 1, msg);
    }
}



MUSIC_PLAYER *music_player_creat(void)
{
    tbool ret = true;
    int err;

    memset((u8 *)&music_player_hdl, 0x0, sizeof(MUSIC_PLAYER));
    MUSIC_PLAYER *obj = (MUSIC_PLAYER *)&music_player_hdl;

    ///music op creat
    obj->dop = music_decoder_creat();
    if (obj->dop == NULL) {
        music_player_printf("music player creat fail ! fun = %s, line = %d\n", __func__, __LINE__);
        ret = false;
        goto __exit;
    }

    ///file op creat
    obj->fop = file_operate_creat();
    if (obj->fop == NULL) {
        music_player_printf("file operate creat fail ! fun = %s, line = %d\n", __func__, __LINE__);
        ret = false;
        goto __exit;
    }


    music_decoder_set_data_clear_interface(obj->dop, NULL, NULL);
    music_decoder_set_data_wait_interface(obj->dop, NULL, NULL);
    music_decoder_set_setInfo_interface(obj->dop, music_player_set_info, NULL);
    music_decoder_set_output_interface(obj->dop, music_player_output, NULL);
    music_decoder_set_err_deal_interface(obj->dop, music_player_decoder_err_deal, obj);
    music_decoder_set_decoder_type(obj->dop, DEC_PHY_MP3 | DEC_PHY_WAV);
    file_operate_set_file_ext(obj->fop, "MP3WAV");
    file_operate_set_repeat_mode(obj->fop, REPEAT_ALL);


// PLAY_FIRST_FILE
    file_operate_set_file_sel_mode(obj->fop, PLAY_FIRST_FILE);
// PLAY_LAST_FILE
    /* file_operate_set_file_sel_mode(obj->fop, PLAY_LAST_FILE); */
//PLAY_SPEC_FILE
    /* file_operate_set_file_sel_mode(obj->fop, PLAY_SPEC_FILE); */
    /* file_operate_set_file_number(obj->fop, 3); */
//PLAY_FILE_BYPATH
    /* file_operate_set_file_sel_mode(obj->fop, PLAY_FILE_BYPATH); */
    /* file_operate_set_path(obj->fop, (void *)"/test.mp3", 0); */


__exit:
    if (ret == false) {
        music_player_destroy(&obj);
    }

    return obj;
}

void music_player_destroy(MUSIC_PLAYER **hdl)
{
    int err;
    if (hdl == NULL || *hdl == NULL) {
        return;
    }
    MUSIC_PLAYER *obj = *hdl;

    music_player_save_break_point(obj);
    //music op destroy
    music_decoder_destroy(&(obj->dop));
    //file op destroy
    file_operate_destroy(&(obj->fop));
#if 0
    free(obj);
#endif
    *hdl = NULL;
}

static tbool music_player_err_deal(MUSIC_PLAYER *obj, u32 err)
{
    tbool ret = false;
    if (obj == NULL) {
        return false;
    }

    music_player_printf("----------------err = %x---------------\n", err);

    if (err == MUSIC_DECODER_ERR_START_FAIL) {
        ///音乐开始解码不成功， 例如格式检查不对等， 认为文件错位处理，执行错误文件统计
        err = FILE_OP_ERR_OPEN_FILE;
    }

    if (err < MUSIC_DECODER_ERR_NONE) {
        ret = file_operate_err_deal(obj->fop, err);
    }

    return ret;
}




tbool music_player_play(MUSIC_PLAYER *obj, MUSIC_PLAYER_BP *bp_info, u8 is_auto)
{
    MUSIC_PLAYER_BP bp_info_rd;
    tbool ret = false;
    u32 err;
    if (obj == NULL) {
        return false;
    }
    //save break point before decode stop
    if (file_operate_get_dev_sel_mode(obj->fop) != DEV_SEL_CUR) {
        music_player_save_break_point(obj);
    }

    ///stop dec
    music_decoder_stop(obj->dop);

    music_player_printf("music_player_play\n");

    vm_check_api(1);
    do {
        //read break
        ///dev sel
        err = file_operate_dev_sel(obj->fop);
        if (!err) {
            if (bp_info == NULL) {
                if (music_player_read_break_point(obj, &bp_info_rd) == true) {
                    bp_info = &bp_info_rd;
                }
            }
            //get_break_point
            err = file_operate_dev_bp(obj->fop, &(bp_info->f_info));

            if (!err) {
                ///file sel
                if (bp_info) {
                    err = file_operate_file_sel(obj->fop, &(bp_info->f_info));
                } else {
                    err = file_operate_file_sel(obj->fop, NULL);
                }
                if (!err) {
                    ///dec
                    music_decoder_set_file_interface(obj->dop, (DEC_FILE_IO *)&music_player_file, file_operate_get_file_hdl(obj->fop));
                    if (bp_info) {
                        music_decoder_set_break_point(obj->dop, bp_info->buf, bp_info->bp_size);
                    } else {
                        music_decoder_set_break_point(obj->dop, NULL, 0);
                    }

                    err = music_decoder_play(obj->dop);

                    if (err == MUSIC_DECODER_ERR_NONE) {
                        music_player_printf("music_player_play ok !! fun = %s, line = %d\n", __func__, __LINE__);
                        return true;
                    } else {
                        music_player_printf("music_player_play dec fail %x !! fun = %s, line = %d\n", err, __func__, __LINE__);
                    }
                }
            }
        }

        if (is_auto) {
            ret = music_player_err_deal(obj, err);
            if (ret == false) {
                music_player_printf("music_player_play fail !! fun = %s, line = %d\n", __func__, __LINE__);
                return false;
            } else {
                music_player_printf("auto err deal continue !!, fun = %s, line = %d\n", __func__, __LINE__);
            }
        }
    } while (is_auto);

    music_player_printf("music_player_play fail !! fun = %s, line = %d\n", __func__, __LINE__);
    return false;
}

tbool music_player_operation(MUSIC_PLAYER *obj, ENUM_FILE_SELECT_MODE op)
{
    if (obj == NULL) {
        return false;
    }

    otp_printf("music_player_operation, %x\n", op);

    file_operate_set_file_sel_mode(obj->fop, op);

    return music_player_play(obj, NULL, 1);
}

tbool music_player_play_spec_file(MUSIC_PLAYER *obj, u32 filenum)
{
    if (obj == NULL) {
        return false;
    }

    file_operate_set_file_sel_mode(obj->fop, PLAY_SPEC_FILE);
    file_operate_set_file_number(obj->fop, filenum);
    return music_player_play(obj, NULL, 1);
}

tbool music_player_play_path_file(MUSIC_PLAYER *obj, u8 *path, u32 index)
{
    if (obj == NULL) {
        return false;
    }

    file_operate_set_file_sel_mode(obj->fop, PLAY_FILE_BYPATH);
    file_operate_set_path(obj->fop, path, index);
    /* file_operate_set_file_number(obj->fop, index); */
    return music_player_play(obj, NULL, 1);
}


tbool music_player_play_spec_dev(MUSIC_PLAYER *obj, u32 dev)
{
    if (obj == NULL) {
        return false;
    }
    printf("music_player_play_spec_dev\n");

    file_operate_set_file_sel_mode(obj->fop, PLAY_BREAK_POINT);
    //file_operate_set_file_sel_mode(obj->fop, PLAY_FIRST_FILE);
    file_operate_set_dev_sel_mode(obj->fop, DEV_SEL_SPEC);
    file_operate_set_dev(obj->fop, dev);
    return music_player_play(obj, NULL, 1);
}



tbool music_player_play_next_dev(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return false;
    }

    file_operate_set_file_sel_mode(obj->fop, PLAY_FIRST_FILE);
    file_operate_set_dev_sel_mode(obj->fop, DEV_SEL_NEXT);
    return music_player_play(obj, NULL, 1);
}



tbool music_player_play_prev_dev(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return false;
    }

    file_operate_set_file_sel_mode(obj->fop, PLAY_FIRST_FILE);
    file_operate_set_dev_sel_mode(obj->fop, DEV_SEL_PREV);
    return music_player_play(obj, NULL, 1);
}

tbool music_player_play_first_dev(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return false;
    }

    file_operate_set_file_sel_mode(obj->fop, PLAY_FIRST_FILE);
    file_operate_set_dev_sel_mode(obj->fop, DEV_SEL_FIRST);
    return music_player_play(obj, NULL, 1);
}

tbool music_player_play_last_dev(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return false;
    }

    file_operate_set_file_sel_mode(obj->fop, PLAY_FIRST_FILE);
    file_operate_set_dev_sel_mode(obj->fop, DEV_SEL_LAST);
    return music_player_play(obj, NULL, 1);
}


void music_player_set_repeat_mode(MUSIC_PLAYER *obj, ENUM_PLAY_MODE mode)
{
    if (obj == NULL) {
        return;
    }
    file_operate_set_repeat_mode(obj->fop, mode);
}

void music_player_set_decoder_init_sta(MUSIC_PLAYER *obj, MUSIC_DECODER_ST sta)
{
    if (obj == NULL) {
        return;
    }
    music_decoder_set_ini_sta(obj->dop, sta);
}

void music_player_ff(MUSIC_PLAYER *obj, u8 second)
{
    if (obj == NULL) {
        return;
    }

    music_decoder_fffr(obj->dop, PLAY_MOD_FF, second);
}

void music_player_fr(MUSIC_PLAYER *obj, u8 second)
{
    if (obj == NULL) {
        return;
    }

    music_decoder_fffr(obj->dop, PLAY_MOD_FB, second);
}


MUSIC_DECODER_ST music_player_pp(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return MUSIC_DECODER_ST_STOP;
    }
    return music_decoder_pp(obj->dop);
}


MUSIC_DECODER_ST music_player_get_status(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return MUSIC_DECODER_ST_STOP;
    }

    return music_decoder_get_status(obj->dop);
}

u32 music_player_get_cur_time(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return 0;
    }

    return music_decoder_get_decode_cur_time(obj->dop);
}


u32 music_player_get_total_time(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return 0;
    }

    return music_decoder_get_decode_total_time(obj->dop);
}


void *music_player_get_cur_dev(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return NULL;
    }

    return file_operate_get_dev(obj->fop);

}


tbool music_player_delete_playing_file(MUSIC_PLAYER *obj)
{
    if (obj == NULL) {
        return false;
    }

    music_decoder_stop(obj->dop);
    ///del file
    //scan file

    return true;
}

extern tbool mutex_resource_release(char *resource);
extern tbool mutex_resource_apply(char *resource, int prio, void (*apply_response)(void *priv), void (*release_request)(void *priv), void *priv);
static void music_tone_err_deal(void *priv, u32 err)
{
    u32 msg;
    MUSIC_PLAYER *obj = (MUSIC_PLAYER *)priv;

    if (err) {
        switch (err) {
        case MAD_ERROR_FILE_END:
            music_player_puts("music_tone END ok\n");
            if (tone_var.rpt_mode == 0) {
                mutex_resource_release("tone");
                msg = SYS_EVENT_PLAY_SEL_END;
                task_post_msg(NULL, 1, msg);
            } else {
                tone_var.status = 0;
            }
            break;

        case MAD_ERROR_SYNC_LIMIT:
        case MAD_ERROR_NODATA:
            music_player_puts("music_tone NODATA ok\n");
            mutex_resource_release("tone");
            msg = SYS_EVENT_PLAY_SEL_END;
            task_post_msg(NULL, 1, msg);
            break;
        default:
            mutex_resource_release("tone");
            printf("music_tone END err:0x%x\n", err);
            break;
        }
    }
}


static u8 tone_back_vol_l, tone_back_vol_r;
void music_tone_play(void *name)
{
    tbool ret = true;
    int err;
    music_player_puts("\n====play music tone start1===\n");
    u8 timeout_cnt = 0;
    /*
     *10ms * 100 = 1000ms
     *播提示音之前等待其他数据播放完毕，防止切换采样率导致之前的数据播放不正确
     *超时这个时间不要乱改，除非你知道是什么意思
     */
    while (get_dac_cbuf_len()) {
        //printf("cnt:%d,len:%d\n",timeout_cnt,get_dac_cbuf_len());
        timeout_cnt++;
        extern void delay_2ms(u32 delay_time);
        delay_2ms(5);
        if (timeout_cnt > 100) {
            puts("wait dac idle timeout\n");
            break;
        }
    }
    dac_toggle(1);


    set_sbc_energy_detect_flag(0);
    dac_channel_off(LINEIN_CHANNEL, 0);
    dac_channel_on(MUSIC_CHANNEL, 0);
#if TONE_DEFAULT_VOL
    tone_back_vol_l = dac_ctl.sys_vol_l;
    tone_back_vol_r = dac_ctl.sys_vol_r;
    dac_ctl.sys_vol_l =  TONE_DEFAULT_VOL;
    dac_ctl.sys_vol_r =  TONE_DEFAULT_VOL;
#endif
    //set_sys_vol(sound.vol.sys_vol_l, sound.vol.sys_vol_r, 1);
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
    memset((u8 *)&music_player_hdl, 0x0, sizeof(MUSIC_PLAYER));
    MUSIC_PLAYER *obj = (MUSIC_PLAYER *)&music_player_hdl;
    tone_var.status = 1;

    ///music op creat
    obj->dop = music_decoder_creat();
    if (obj->dop == NULL) {
        music_player_printf("music player creat fail ! fun = %s, line = %d\n", __func__, __LINE__);
        ret = false;
        goto __exit;
    }

    ///file op creat
    obj->fop = file_operate_creat();
    if (obj->fop == NULL) {
        music_player_printf("file operate creat fail ! fun = %s, line = %d\n", __func__, __LINE__);
        ret = false;
        goto __exit;
    }

    music_decoder_set_data_clear_interface(obj->dop, NULL, NULL);
    music_decoder_set_data_wait_interface(obj->dop, NULL, NULL);
    music_decoder_set_setInfo_interface(obj->dop, music_player_set_info, NULL);
    music_decoder_set_output_interface(obj->dop, music_player_output, NULL);
    music_decoder_set_err_deal_interface(obj->dop, music_tone_err_deal, obj);
    music_decoder_set_decoder_type(obj->dop, DEC_PHY_MP3 | DEC_PHY_WAV);
    file_operate_set_file_ext(obj->fop, "MP3WAV");

    file_operate_set_repeat_mode(obj->fop, REPEAT_ONE);

    file_operate_set_file_sel_mode(obj->fop, PLAY_FILE_BYPATH);
    file_operate_set_path(obj->fop, (void *)name, 0);

    file_operate_set_dev_sel_mode(obj->fop, DEV_SEL_SPEC);
    file_operate_set_dev(obj->fop, (u32)cache);
    ret = music_player_play(obj, NULL, 0);
    if (ret == false) {
        puts(">>>music_tone play faild\n");
    }

__exit:
    if (ret == false) {
        music_tone_err_deal(obj, MAD_ERROR_NODATA);
        set_sbc_energy_detect_flag(1);
    }
}

void music_tone_end(void)
{
    music_player_puts("\n====play music tone end===\n");
    MUSIC_PLAYER *obj = (MUSIC_PLAYER *)&music_player_hdl;
    music_decoder_stop(obj->dop);
    set_sbc_energy_detect_flag(1);
    music_player_destroy(&obj);
    tone_var.status = 0;
#if TONE_DEFAULT_VOL
    dac_ctl.sys_vol_l = tone_back_vol_l;
    dac_ctl.sys_vol_r = tone_back_vol_r;
#endif
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
}

void music_tone_stop(void)
{
    music_player_puts("\n====stop play music tone===\n");
    mutex_resource_release("tone");
}




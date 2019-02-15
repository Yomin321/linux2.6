#include "sdk_cfg.h"
#include "common/msg.h"
#include "common/includes.h"
#include "common/common.h"
#include "encode/encode.h"
#include "rec_api.h"
#include "rec_play.h"
#include "audio_param.h"
#include "task_manager.h"
#include "rec_file_op.h"
#include "irq_api.h"

/* #include "resource_manage.h" */
extern void resource_manage_schedule();
extern tbool mutex_resource_release(char *resource);
extern tbool mutex_resource_apply(char *resource, int prio, void (*apply_response)(void *priv), void (*release_request)(void *priv), void *priv);

static bool rec_encode_loop_en = true;
void rec_set_loop_en(bool en)
{
    rec_encode_loop_en = en;
}

#if REC_EN
/* static u8 rec_alloc_pool[REC_POOL_SIZE] sec_used(.rec_mem_pool); */
extern u32 music_decode_ram[5 * 1024];

REC_FILE_INFO last_rec_file;

SET_INTERRUPT
void rec_encode_soft_irq_loop()
{
    irq_common_handler(IRQ_SOFT_REC_IDX);
    encode_run();  //运行编码
    enc_out_run();  //编码输出处理
}

static void rec_encode_soft_irq_resume()
{
    if (rec_encode_loop_en == false) {
        return;
    }
    irq_set_pending(IRQ_SOFT_REC_IDX);
}

/*----------------------------------------------------------------------------*/
/**@brief  获取录音时间
   @param  录音句柄
   @return 时间
   @note
*/
/*----------------------------------------------------------------------------*/
u32 rec_get_enc_time(RECORD_OP_API *rec_api)
{
    if (rec_api) {
        return updata_enc_time(rec_api->enc_ctl);
    }
    return 0;
}

u32 rec_get_enc_sta(RECORD_OP_API *rec_api)
{
    if (rec_api) {
        return encode_get_status(rec_api->enc_ctl);
    }
    return 0;
}

void *rec_get_cur_dev(RECORD_OP_API *rec_api)
{
    if (rec_api) {
        return rec_fop_get_dev(rec_api->output_hd);
    }
    return NULL;
}
/*----------------------------------------------------------------------------*/
/**@brief  录音输入完成回调
   @param  priv
   @return NULL
   @note
*/
/*----------------------------------------------------------------------------*/
static void rec_encode_input_end_cbk(void *priv)
{
    rec_encode_soft_irq_resume();  //输入数据完成，起软中断处理
}

/*----------------------------------------------------------------------------*/
/**@brief  录音编码错误处理
   @param  err
   @return NULL
   @note   中断中回调，尽量少打印
*/
/*----------------------------------------------------------------------------*/
static void rec_encode_err_deal(u32 err)
{
    switch (err) {
    case ERR_ENCODE_OUT_ERR:
        task_post_msg(NULL, 1, MSG_REC_STOP);
        break;
    case ERR_ENCODE_IN_LOST_FRAME:
        task_post_msg(NULL, 1, MSG_REC_INPUT_ERR);
        break;
    case ERR_ENCODE_OUT_LOST_FRAME:
        task_post_msg(NULL, 1, MSG_REC_OUTPUT_ERR);
        break;
    case ERR_ENCODE_RUN_ERR:
        task_post_msg(NULL, 1, MSG_REC_STOP);
        break;
    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  录音接口初始化
   @param  ch录音通道
   @return 录音句柄
   @note
*/
/*----------------------------------------------------------------------------*/
RECORD_OP_API  *rec_init(u8 ch)
{
    RECORD_OP_API *rec_op_api = NULL;
    u32 sr;
    u8 ch_cnt;

    rec_api_printf("fun = %s, line = %d\n", __func__, __LINE__);
#if (REC_MALLOC == 0)
    /* rec_alloc_init(rec_alloc_pool, REC_POOL_SIZE);  //录音内存资源初始化 */
    rec_alloc_init(music_decode_ram, sizeof(music_decode_ram));  //录音内存资源初始化
#endif

    switch (ch) {
    case REC_MIC_CHANNEL:
        sr = SR48000;
        ch_cnt = 1;
        break;
    case REC_LINEIN_CHANNEL:
        sr = SR44100;
        ch_cnt = 1;
        break;
    default:
        return NULL;
    }

    /**********************打开录音输出模块*********************************/
    rec_api_printf("fun = %s, line = %d\n", __func__, __LINE__);
    rec_op_api = rec_malloc(sizeof(RECORD_OP_API));
    REC_ASSERT(rec_op_api, __rec_op_err);

    rec_api_printf("fun = %s, line = %d\n", __func__, __LINE__);
    rec_op_api->output_hd = rec_out_init();
    REC_ASSERT(rec_op_api->output_hd, __rec_out_init_err);

    /**********************打开编码器*********************************/

    encode_err_deal_cbk_register(rec_encode_err_deal); //注册错误处理回调
    encode_input_end_cbk_register(rec_encode_input_end_cbk);   //编码器输入完成回调注册
#if REC_FILE
    //MP2_FORMAT
    //(44100), (48000), (32000) 采样率可配置以下比特率
    //br:32,48,56,64,80,96,112,128,160,192,224,256,320,384
    //(44.1K/2,/4),  (48k/2,/4),  (32K/2,/4) 采样率可配置以下比特率
    //br:8,16,24,32,40,48,56,64,80,96,112,128,144,160
    rec_op_api->enc_ctl = encode_open(ch_cnt, 128, sr, MP2_FORMAT);  //启动录音编码
#else
    rec_op_api->enc_ctl = encode_open(ch_cnt, 1024, sr, ADPCM_FORMAT);  //启动录音编码
#endif
    REC_ASSERT(rec_op_api->enc_ctl, __encode_open_err);

    rec_api_printf("fun = %s, line = %d\n", __func__, __LINE__);
    rec_op_api->enc_ctl->output_io.priv = rec_op_api->output_hd;
    rec_op_api->enc_ctl->output_io.seek = rec_out_seek;
    rec_op_api->enc_ctl->output_io.tell = rec_out_tell;
    rec_op_api->enc_ctl->output_io.output = rec_out_put;

    //使用软中断跑编码流程
    irq_handler_register(IRQ_SOFT_REC_IDX, rec_encode_soft_irq_loop, irq_index_to_prio(IRQ_SOFT_REC_IDX));

    enc_run_start(rec_op_api->enc_ctl);  //启动编码

    /**********************打开录音输入模块*********************************/
    rec_op_api->input_hd = rec_input_init(&rec_op_api->enc_ctl->input_io, ch, sr);
    REC_ASSERT(rec_op_api->input_hd, __rec_input_init_err);

    rec_api_printf("fun = %s, line = %d\n", __func__, __LINE__);
    return rec_op_api;

__rec_input_init_err:
    encode_close(rec_op_api->enc_ctl);
__encode_open_err:
    rec_out_exit(&rec_op_api->output_hd);
__rec_out_init_err:
    rec_free_fun((void **)&rec_op_api);
__rec_op_err:
    return NULL;
}

/*----------------------------------------------------------------------------*/
/**@brief  退出录音
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void rec_exit(RECORD_OP_API **rec_api_p)
{

    if (rec_api_p && *rec_api_p) {
        rec_get_file_info((*rec_api_p)->output_hd, &last_rec_file);
        rec_input_exit(&(*rec_api_p)->input_hd);
        encode_close((*rec_api_p)->enc_ctl);
        rec_out_exit(&(*rec_api_p)->output_hd);
        rec_free_fun((void **)rec_api_p);
    }

    irq_handler_unregister(IRQ_SOFT_REC_IDX);
    mutex_resource_release("recode");
}


void recode_mutex_init(void *priv)
{
    RECORD_OP_API **rec_api_p = priv;

    switch (task_get_cur()) {
#if 0
    case TASK_ID_LINEIN:
        *rec_api_p = rec_init(REC_LINEIN_CHANNEL);
        break;
#endif
#if REC_TASK_EN
    case TASK_ID_REC:
        *rec_api_p = rec_init(REC_MIC_CHANNEL);
        break;
#endif
    default:
        return;
    }
}

void recode_mutex_stop(void *priv)
{
    RECORD_OP_API **rec_api_p = priv;
    if (*rec_api_p) {
        rec_exit(rec_api_p);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  录音消息处理
   @param  rec_api_p 录音句柄指针  msg 消息
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void rec_msg_deal(RECORD_OP_API **rec_api_p, int msg)
{
    switch (msg) {
    case MSG_REC_START:
        rec_api_printf("MSG_REC_START");
        if (rec_api_p == NULL) {
            return;
        }

        if (*rec_api_p) {
            rec_exit(rec_api_p);
            resource_manage_schedule();
        } else {
            mutex_resource_apply("recode", 4, recode_mutex_init, recode_mutex_stop, rec_api_p);
        }
        break;

    case MSG_REC_INPUT_ERR:
        printf("encode input buf err \n");
        break;
    case MSG_REC_OUTPUT_ERR:
        printf("encode output buf err n");
        break;
    case MSG_REC_STOP:
        if (rec_api_p && (*rec_api_p)) {
            rec_exit(rec_api_p);
        }
        printf("rec stop\n");
        break;

    case MSG_REC_PP:
        if (rec_api_p && (*rec_api_p)) {
            encode_pp((*rec_api_p)->enc_ctl);
        }
        break;

#if REC_DEL
    case MSG_REC_DEL:
        rec_api_printf("MSG_REC_DEL\n");
        recode_play_mutex_stop(NULL);
        if (rec_api_p && (*rec_api_p)) {
            rec_exit(rec_api_p);
        }
        if (rec_fop_del_recfile(&last_rec_file)) {
            printf("del rec file err\n");
        }
        rec_fop_update_lastfile(&last_rec_file);
        break;
#endif

#if REC_PLAY_EN
    case MSG_REC_PLAY:
        rec_api_printf("MSG_REC_PLAY\n");
        if (rec_api_p && (*rec_api_p)) {
            rec_exit(rec_api_p);
        }
        rec_play_api(&last_rec_file);
        break;

    case MSG_REC_PLAY_STOP:
    case MSG_REC_PLAY_END:
    case MSG_REC_PLAY_ERR:
        recode_play_mutex_stop(NULL);
        break;
#endif
    default:
        break;
    }
}

#endif

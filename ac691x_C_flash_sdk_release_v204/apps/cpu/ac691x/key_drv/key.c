#include "key.h"

#include "key_drv_io.h"
#include "key_drv_ad.h"
#include "key_drv_ir.h"
#include "key_drv_uart.h"
#include "key_drv_tch.h"

#include "tone.h"

#include "timer.h"
#include "msg.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".key_bss")
#pragma data_seg(	".key_data")
#pragma const_seg(	".key_const")
#pragma code_seg(	".key_code")
#endif

extern u8 is_enter_fast_test(void);

static u8 key_type;        ///<按键类型
static const KEY_REG *cur_key_msg_reg = NULL;
volatile static u8 key_invalid_flag = 0;

static const key_interface_t *key_list[] = {

#if (KEY_AD_RTCVDD_EN||KEY_AD_VDDIO_EN)
    &key_ad_info,
#endif

#if KEY_IO_EN
    &key_io_info,
#endif

#if KEY_IR_EN
    &key_ir_info,
#endif

#if KEY_TCH_EN
    &key_touch_info,
#endif

#if KEY_UART_EN
    &key_uart_info,
#endif
};

u8 __get_key_invalid_flag(void)
{
    return key_invalid_flag;
}

void key_init(void)
{
    u8 i;

    key_puts("key init\n");

    for (i = 0; i < (sizeof(key_list) / sizeof(key_list[0])); i++) {
        if (key_list[i]->key_init) {
            key_list[i]->key_init();
        }
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   按键-消息转换函数
   @param   key_status：按键状态
   @param   back_last_key：最后按键值
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void key2msg_filter(u8 key_status, u8 back_last_key)
{
    u16 msg = NO_MSG;

    if (is_enter_fast_test()) {
        tone_play(500);
    }

#if KEY_IR_EN
    if (KEY_TYPE_IR == key_type) {
        if (cur_key_msg_reg != NULL && cur_key_msg_reg->_ir != NULL) {
            msg = cur_key_msg_reg->_ir[key_status][back_last_key];
        } else {
            return ;
        }
    }
#endif

#if KEY_IO_EN
    if (KEY_TYPE_IO == key_type) {
        if (cur_key_msg_reg != NULL && cur_key_msg_reg->_io != NULL) {
            msg = cur_key_msg_reg->_io[key_status][back_last_key];
        } else {
            return ;
        }
    }
#endif

#if KEY_TCH_EN
    if (KEY_TYPE_TOUCH == key_type) {
        if (cur_key_msg_reg != NULL && cur_key_msg_reg->_touch != NULL) {
            msg = cur_key_msg_reg->_touch[key_status][back_last_key];
        } else {
            return ;
        }
    }
#endif // KEY_TCH_EN

#if (KEY_AD_RTCVDD_EN||KEY_AD_VDDIO_EN)
    if (KEY_TYPE_AD == key_type) {
        if (cur_key_msg_reg != NULL && cur_key_msg_reg->_ad != NULL) {
            msg = cur_key_msg_reg->_ad[key_status][back_last_key];
        } else {
            return ;
        }
    }
#endif

#if KEY_UART_EN
    if (KEY_TYPE_UART == key_type) {
        if (cur_key_msg_reg != NULL && cur_key_msg_reg->_uart != NULL) {
            msg = cur_key_msg_reg->_uart[key_status][back_last_key];
        } else {
            return ;
        }
    }
#endif // KEY_TCH_EN

    otp_printf("key:%d\tmsg:0x%02x\n", back_last_key, msg);
    task_post_msg(NULL, 1, msg);
}

/*----------------------------------------------------------------------------*/
/**@brief   按键-消息转换函数,按键产生顺序：短按抬起/长按-->连按
   @param
   @param
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
u8 get_key_value(void)
{
    u8 i;
    u8 key_num;

    for (i = 0; i < (sizeof(key_list) / sizeof(key_list[0])); i++) {
        if (key_list[i]->key_get_value) {
            key_num = key_list[i]->key_get_value();

            if (NO_KEY != key_num) {
                key_type = key_list[i]->key_type;
                /* otp_printf("get_key_num = %d\n", key_num); */
                return key_num;
            }
        }
    }
    return NO_KEY;
}

/*----------------------------------------------------------------------------*/
/**@brief   按键-消息转换函数,按键产生顺序：短按抬起/长按-->连按
   @param
   @param
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void key_scan(void *ptr)
{
    static u8 last_key = NO_KEY;
    static u8 key_press_counter = 0;
    static u8 key_press_flag = 0;
    static u8 double_last_key = 0;
    u8 cur_key, key_status, back_last_key;

    /* key_voice_dac_channel(); */

    if (key_press_flag) {
        key_press_flag++;
        if (key_press_flag > KEY_DOUBLE_CLICK_CNT) {
            key_puts(" One_Click ");
            key_press_flag = 0;
            key2msg_filter(KEY_SHORT_UP, double_last_key);
        }
    }

    cur_key = NO_KEY;
    back_last_key = last_key;
    cur_key = get_key_value();

    if (cur_key != NO_KEY) {
        key_invalid_flag = 1;
    } else {
        key_invalid_flag = 0;
    }

#if 0
    if (cur_key != NO_KEY) {
        otp_printf("key : %d\n", cur_key);
    }
#endif

    if (cur_key == last_key) {                          //长时间按键
        if (cur_key == NO_KEY) {
            return;
        }
        key_press_counter++;
        if (key_press_counter == KEY_LONG_CNT) {        //长按
            key_status = KEY_LONG;
        } else if (key_press_counter == (KEY_LONG_CNT + KEY_HOLD_CNT)) {    //连按
            key_status = KEY_HOLD;
            key_press_counter = KEY_LONG_CNT;
        } else {
            return;
        }
    } else { //cur_key = NO_KEY, 抬键
        last_key = cur_key;
        if ((key_press_counter < KEY_LONG_CNT) && (cur_key == NO_KEY)) {    //短按抬起
            key_press_counter = 0;
            key_status = KEY_SHORT_UP;
        } else if ((cur_key == NO_KEY) && (key_press_counter >= KEY_LONG_CNT)) { //长按抬起
            key_press_counter = 0;
            key_status = KEY_LONG_UP;
            //puts("[UP]");
        } else {
            key_press_counter = 0;
            return;
        }
    }

    //otp_printf("key_status:0x%02x	back_last_key:%02x\n", key_status, back_last_key);
#if KEY_DOUBLE_CLICK

#if (KEY_DOUBLE_CLICK==KEY_DOUBLE_CLICK_PP)
    if ((back_last_key == 2) && (key_status == KEY_SHORT_UP)) {//双击pp键
#elif (KEY_DOUBLE_CLICK&(KEY_DOUBLE_CLICK_PP|KEY_DOUBLE_CLICK_VOL_DOWN))
    if (((back_last_key == 1) || (back_last_key == 2)) && (key_status == KEY_SHORT_UP)) { //双击音量减和pp键
#elif (KEY_DOUBLE_CLICK&(KEY_DOUBLE_CLICK_PP|KEY_DOUBLE_CLICK_VOL_UP))
    if (((back_last_key == 0) || (back_last_key == 2)) && (key_status == KEY_SHORT_UP)) { //双击音量加和pp键
#else
    if (((back_last_key == 0) || (back_last_key == 1) || (back_last_key == 2)) && (key_status == KEY_SHORT_UP)) { //双击音量加和pp键音量减
#endif

        if (key_press_flag == 0) {
            key_press_flag = 1;
            double_last_key = back_last_key;
        }
        if ((key_press_flag > 15) && (key_press_flag <= KEY_DOUBLE_CLICK_CNT)) {
            //key_puts(" Double_Click ");
            key_press_flag = 0;
            if (back_last_key == 2) { //双击pp键
#if (KEY_DOUBLE_CLICK&KEY_DOUBLE_CLICK_PP)
                key2msg_filter(key_status, 9);
#endif
            }
#if (KEY_DOUBLE_CLICK&KEY_DOUBLE_CLICK_VOL_DOWN)
            else if (back_last_key == 1) { //双击音量减
                key2msg_filter(key_status, 8);
            }
#endif
#if (KEY_DOUBLE_CLICK&KEY_DOUBLE_CLICK_VOL_UP)
            else if (back_last_key == 0) { //双击音量加
                key2msg_filter(key_status, 8);
            }
#endif
        }
    } else
#endif
    {
        key2msg_filter(key_status, back_last_key);
    }
}



void key_msg_reg(const KEY_REG *reg)
{
    CPU_SR_ALLOC();
    CPU_INT_DIS();
    cur_key_msg_reg = reg;
    CPU_INT_EN();
    otp_printf("---------------cur_key_msg_reg = 0x%x------------------\n", cur_key_msg_reg);
}



#ifndef _KEY_DRV_TCH_H_
#define _KEY_DRV_TCH_H_

#include "key.h"

//touch key number
#define TOUCH_KEY_CH_MAX	1

typedef struct _CTM_KEY_VAR {
    s32 touch_release_buf[TOUCH_KEY_CH_MAX]; 		//按键释放值滤波器buffer
    u16 touch_cnt_buf[TOUCH_KEY_CH_MAX];			//按键计数值滤波器buffer
    s16 FLT1CFG1;					//滤波器1配置参数1
    s16 FLT1CFG2;					//滤波器1配置参数2, 等于(-RELEASECFG0)<<FLT1CFG0
    s16 PRESSCFG;					//按下判决门限
    s16 RELEASECFG0;				//释放判决门限0
    s16 RELEASECFG1;				//释放判决门限1
    s8  FLT0CFG;					//滤波器0配置参数(0/1/2/3)
    s8  FLT1CFG0;					//滤波器1配置参数0
    u16 touch_key_state;			//按键状态标志，随时可能被中断改写，按键处理程序需要将此标志复制出来再行处理
    u8  touch_init_cnt[TOUCH_KEY_CH_MAX];				//初始化计数器，非0时进行初始化
} sCTM_KEY_VAR;

u8 get_tchkey_value(void);
void tch_key_init(void);


extern const key_interface_t key_touch_info;


#endif  /*  _TOUCH_KEY_API_H_  */



#include "key.h"
#include "key_drv_tch.h"
#include "string.h"

#include "uart.h"

#if KEY_TCH_EN

u8 g_touch_len;        //触摸按键数
u8  bCap_ch;            //触摸通道
u16 Touchkey_value_new = 0;
u16 Touchkey_value_old = 0;
sCTM_KEY_VAR ctm_key_value;
sCTM_KEY_VAR *ctm_key_var;

void ctm_key_var_init(sCTM_KEY_VAR *ptr)
{
    ctm_key_var = ptr;
}

static void ctm_irq(u16 ctm_res, u8 ch)
{
    u16 temp_u16_0, temp_u16_1;
    s16 temp_s16_0, temp_s16_1;
    s32 temp_s32_0;
//..............................................................................................
//取计数值/通道判断
//..............................................................................................


    if (ctm_key_var->touch_init_cnt[ch]) {
        ctm_key_var->touch_init_cnt[ch]--;
//		touch_cnt_buf[ch] = rvalue << FLT0CFG;
//		touch_release_buf[ch] = (long)(rvalue) << FLT1CFG0;
        ctm_key_var->touch_cnt_buf[ch] = (u32)ctm_res << ctm_key_var->FLT0CFG;
        ctm_key_var->touch_release_buf[ch] = (u32)ctm_res << ctm_key_var->FLT1CFG0;
    }

//..............................................................................................
//当前计数值去抖动滤波器
//..............................................................................................
    temp_u16_0 = ctm_key_var->touch_cnt_buf[ch];
    temp_u16_1 = temp_u16_0;
    temp_u16_1 -= (temp_u16_1 >> ctm_key_var->FLT0CFG);
    temp_u16_1 += ctm_res;//temp_u16_1 += rvalue;
    ctm_key_var->touch_cnt_buf[ch] = temp_u16_1;
    temp_u16_0 += temp_u16_1;
    temp_u16_0 >>= (ctm_key_var->FLT0CFG + 1);


//..............................................................................................
//各通道按键释放计数值滤波器
//..............................................................................................
    temp_s32_0 = ctm_key_var->touch_release_buf[ch];
    temp_u16_1 = temp_s32_0 >> ctm_key_var->FLT1CFG0;	//获得滤波器之后的按键释放值
    temp_s16_0 = temp_u16_0 - temp_u16_1;	//获得和本次检测值的差值，按下按键为负值，释放按键为正值
    temp_s16_1 = temp_s16_0;

    /* if(ch == 0) */
    /* { */
    /* otp_printf("ch%d: %d  %d", (short)ch, temp_u16_0, temp_s16_1); */
    /* } */

    if (ctm_key_var->touch_key_state & BIT(ch)) {	//如果本通道按键目前是处于释放状态
        if (temp_s16_1 >= 0) {	//当前计数值大于低通值，放大后参与运算
            if (temp_s16_1 < (ctm_key_var->FLT1CFG2 >> 3)) {
                temp_s16_1 <<= 3;	//放大后参与运算
            } else {
                temp_s16_1 = ctm_key_var->FLT1CFG2;	//饱和，防止某些较大的正偏差导致错判
            }
        } else if (temp_s16_1 >= ctm_key_var->FLT1CFG1) {	//当前计数值小于低通值不多，正常参与运算
        } else {			//当前计数值小于低通值很多，缩小后参与运算 (有符号数右移自动扩展符号位???)
            temp_s16_1 >>= 3;
        }
    } else {		//如果本通道按键目前是处于按下状态, 缓慢降低释放计数值
        if (temp_s16_1 <= ctm_key_var->RELEASECFG1) {
            temp_s16_1 >>= 3;		//缩小后参与运算
        } else {
            temp_s16_1 = 0;
        }
    }

    temp_s32_0 += (s32)temp_s16_1;
    ctm_key_var->touch_release_buf[ch] = temp_s32_0;

//..............................................................................................
//按键按下与释放检测
//..............................................................................................
    if (temp_s16_0 <= ctm_key_var->PRESSCFG) {			//按键按下
        ctm_key_var->touch_key_state &= ~BIT(ch);
    } else if (temp_s16_0 >= ctm_key_var->RELEASECFG0) {	//按键释放
        ctm_key_var->touch_key_state |= BIT(ch);
    }
}


u8 get_tchkey_value(void)
{
    u8 key;
    u8 i;

    for (i = 0; i < g_touch_len; i++) {
        if (!(ctm_key_value.touch_key_state & (u8)(BIT(i)))) {
            break;
        }
    }
    key = (i < g_touch_len) ? i : NO_KEY;

    /* if(key != NO_KEY){ */
    /* otp_printf("tch %x", key); */
    /* } */

    return key;
}



void set_port_out(u8 chan)
{
    switch (chan) {
    case 0:
        JL_PORTA->DIR &= ~BIT(0);
        break;
    }
}

void set_port_out_H(u8 chan)
{
    switch (chan) {
    case 0:
        JL_PORTA->OUT |= BIT(0);
        break;
    }
}

void set_port_pd(u8 chan)
{
    switch (chan) {
    case 0:
        JL_PORTA->PD |= BIT(0);
        break;
    }
}

void set_port_in(u8 chan)
{
    switch (chan) {
    case 0:
        JL_PORTA->DIR |= BIT(0);
        break;
    }
}

u8 g_touch_port(u8 chan)
{
    u8 first_chan = 0;		//if select PORTA, first_chan is 0,  PORTB‘s first_chan is 16	PORTC's first_chan is 32
    return (first_chan + chan);
}

void set_touch_io(u8 chan_id)
{
    JL_IOMAP->CON2 &= ~(0xff0000);
    JL_IOMAP->CON2 |= g_touch_port(chan_id) << 16;   ///选定ＩＯ
    set_port_in(chan_id);///设置为输入
}


void scan_capkey(void)
{
    u16 temp;
    u16 Touchkey_value_delta;

    if (0 == g_touch_len) {
        return;
    }

    Touchkey_value_new = JL_PLL_COUNTER->TVL;   ///获取计数值

    set_port_out_H(bCap_ch);//set output H
    set_port_out(bCap_ch);//set output
    //***wait IO steady for pulse counter

    if (Touchkey_value_old > Touchkey_value_new) {
        Touchkey_value_new += 0x10000;
    }

    Touchkey_value_delta = Touchkey_value_new - Touchkey_value_old;
    Touchkey_value_old = Touchkey_value_new;	///记录旧值
    temp = 6800L - Touchkey_value_delta * 1;    ///1变化增大倍数

    /* if(bCap_ch == 0){ */
    /* otp_printf("old: %x   new: %x", Touchkey_value_old, Touchkey_value_new ); */
    /* otp_printf("value: %x\n", Touchkey_value_delta); */
    /* } */

    /* otp_printf("\nTouchkey_value_delta: %d  ch: %d", Touchkey_value_delta, bCap_ch);puts("\n"); */
    /* if(bCap_ch == 0) */
    /* { */
    /* otp_printf("temp: %d\n", Touchkey_value_delta);//puts("\n"); */
    /* } */

    /*调用滤波算法*/
    ctm_irq(temp, bCap_ch);

    /*切换通道，开始充电，PLL CNT 输入Mux 切换*/
    bCap_ch++;
    bCap_ch = (bCap_ch >= g_touch_len) ? 0 : bCap_ch;

    //***make sure IO is steady (IO output high voltage) for pulse counter
    set_touch_io(bCap_ch);
}



void tch_key_init(void)
{
    ctm_key_var_init(&ctm_key_value);

    my_memset((u8 *)&ctm_key_value, 0x0, sizeof(sCTM_KEY_VAR));

    /*触摸按键参数配置*/
    ctm_key_value.FLT0CFG = 0;
    ctm_key_value.FLT1CFG0 = 7;
    ctm_key_value.FLT1CFG1 = -80;
    ctm_key_value.FLT1CFG2 = (-(-10)) << 7; //1280

    ///调节灵敏度的主要参数
    ctm_key_value.PRESSCFG = -10;
    ctm_key_value.RELEASECFG0 = -50;
    ctm_key_value.RELEASECFG1 = -80;//-81;

    my_memset((u8 *) & (ctm_key_value.touch_init_cnt[0]), 0x10, TOUCH_KEY_CH_MAX);

    ctm_key_value.touch_key_state = 0xffff; //<按键默认释放

    ///初始化计数器配置：
    JL_PLL_COUNTER->CON &= ~(0xc);	//

    JL_PLL_COUNTER->CON &= ~(BIT(3) | BIT(2));	//选择PLL 96M时钟计数
    JL_PLL_COUNTER->CON |= BIT(2);	//选择PLL 96M时钟计数
    /* JL_PLL_COUNTER->CON |= (BIT(2) | BIT(3));	//选择PLL 96M时钟计数 */

    JL_PLL_COUNTER->CON |= BIT(1);	//使能使能计数器

    g_touch_len = TOUCH_KEY_CH_MAX;

    Touchkey_value_old = JL_PLL_COUNTER->TVL;   ///获取计数值
    set_port_out_H(bCap_ch);
    set_port_out(bCap_ch);

#if 1       //如果外部有下拉电阻，可不是用芯片内部下拉
    u8 i;
    for (i = 0; i < g_touch_len; i++) {
        set_port_pd(i);
    }
#endif

    puts("touch_key_init \n");
}


const key_interface_t key_touch_info = {
    .key_type = KEY_TYPE_TOUCH,
    .key_init = tch_key_init,
    .key_get_value = get_tchkey_value,
};

#else

void scan_capkey(void)
{
}

#endif


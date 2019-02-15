#ifndef POWER_H
#define POWER_H

#include "typedef.h"
enum {
    POWER_ON_CNT_SET = 1,
    POWER_ON_CNT_INC = 2,
    POWER_ON_CNT_GET = 3,
};
#define GOINT_POWER_OFF_START_CNT 1
#define GOINT_POWER_OFF_END_CNT   4    //控制按键关机时间
#define GOINT_POWER_OFF_START     0XFF //立即关机

u16 get_battery_level(void);
void power_init_app(u8 mode, u8 chargeV);

u32 bt_noconn_pwr_down_in(void);
u32 bt_noconn_pwr_down_out(void);
void battery_check(void);
extern u8 get_going_to_pwr_off();
extern void set_going_to_pwr_off(u8 cnt);
extern void bt_msg_power_off();
extern u16 control_power_on_cnt(u8 mode, u16 poweron_cnt);
void set_main_ldo_en(u8 en);
void pa_umute(void);
void pa_mute(void);
#endif      //POWER_H

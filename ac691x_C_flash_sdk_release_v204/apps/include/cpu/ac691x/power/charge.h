#ifndef _CHARGE_H_
#define _CHARGE_H_

#include "typedef.h"
#include "common/common.h"

enum {
    POWER_ON = 1,
    POWER_OFF,
};
#define POWER_KEY				PORTR2

#define POWER_KEY_INIT()        do{PORTR_PU(POWER_KEY,1);PORTR_PD(POWER_KEY,0);PORTR_DIR(POWER_KEY,1);\
  							      }while(0)
#define IS_POWER_KEY_DOWN()    	(!PORTR_IN(POWER_KEY))



//可配置选项
//充电时间过长，可以调整关机的充电阀值POWEROFF_THRESHOLD_VALUE和开机充电阀值POWERON_THRESHOLD_VALUE宏，值越大，充电时间越短，充满电压越低
#define POWEROFF_THRESHOLD_VALUE        500L
#define POWERON_THRESHOLD_VALUE         580L

#define C_POWER_BAT_CHECK_CNT         50
#define C_POWER_KEY_CHECK_CNT         300

u32 get_ldo5v_online_flag(void);

void charge_mode_detect_ctl(u8 sw);

u8 power_on_check(void);
u8 power_off_check(void);
void poweroff_charge_detect(void);
void poweron_charge_detect(void);
void ldo5v_detect_deal();
bool check_pull_up_value(void);
u16 get_pull_up_value(void);
// void charge_disconnect_bt_check(void);
// void charge_disconnect_bt_check(u16 val);
extern u16 get_cfg_charge_full(void);

#endif    //_CHARGE_H_

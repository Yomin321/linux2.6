#ifndef __TASK_BT_H__
#define __TASK_BT_H__
#include "task_manager.h"

#if TASK_MANGER_ENABLE
extern const TASK_APP task_bt_info;
#endif

void task_bt_deal(void *hdl);
// void task_bt_deal(void);
extern void no_background_suspend();

void power_on_ignore_msg(void);
extern bool check_connect_hid(void);
extern void bt_osc_internal_cfg(u8 sel_l, u8 sel_r);
#endif//__TASK_BT_H__


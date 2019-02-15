#ifndef __KEY_DRV_AD_H__
#define __KEY_DRV_AD_H__

#include "key_drv/key.h"

#define ADC_KEY_NUMBER 10



extern u8 get_adkey_value();

extern void check_ad_up_control(u8 enble);
extern u8 get_pull_up_num();

extern const key_interface_t key_ad_info;
#endif/*__KEY_DRV_AD_H__*/

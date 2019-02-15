#ifndef _SYS_CFG_H_
#define _SYS_CFG_H_

#include "typedef.h"
#include "flash_cfg.h"

#define FLASH_BASE_ADDR		0x1000000UL

enum {
    CFG_ADR_MINIFS	= 0	,
    CFG_ADR_BT_INFO		,
    CFG_ADR_TONE_INFO	,
    CFG_ADR_AEC_INFO	,
    CFG_ADR_LOW_PWR		,
    CFG_ADR_PWR_INFO	,
    CFG_ADR_MAX			,
};
extern u32 *cfg_info_addr[CFG_ADR_MAX];
extern APP_USE_FLASH_SYS_CFG app_use_flash_cfg;

void parse_sys_cfg(u32 cfg_addr);
void cfg_info_init(u32 app_addr, u32 cfg_addr);

#endif

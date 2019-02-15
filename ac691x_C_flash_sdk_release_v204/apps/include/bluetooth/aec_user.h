#ifndef __AEC_USER_H__
#define __AEC_USER_H__

#include "typedef.h"

#define AEC_DEBUG_ONLINE	1
#define AEC_CFG_EN			1

#define AEC_REDUCE		BIT(0)
#define AEC_ADVANCE		BIT(14)

void aec_cfg_init();
u32 aec_param_init();
s8 aec_config_online(void *buf, u16 size);

#endif

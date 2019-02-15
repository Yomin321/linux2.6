#ifndef _A2DP_SYNC_H_
#define _A2DP_SYNC_H_

#include "typedef.h"

#define A2DP_SYNC_EN	1

s32 a2dp_sync_init();
u32 a2dp_sync_run(void *buf, u32 len, u16 sr);
s32 a2dp_sync_exit();

#endif

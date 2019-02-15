#ifndef __AEC_MAIN_H__
#define __AEC_MAIN_H__

#include "typedef.h"
#include "aec/aec.h"
#include "aec/loss_packets_repair.h"

u32 aec_start_api();
u32 aec_stop_api();
void aec_task_main();
void phone_audio_init(u8 en);

s32 hook_sco_conn(void *priv);
s32 hook_sco_disconn(void *priv);
void hook_sco_rx(u8 *data, u16 point, u8 sco_flags);

#endif

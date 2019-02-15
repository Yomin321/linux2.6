#ifndef _LOSS_PACKET_REPAIR_H
#define _LOSS_PACKET_REPAIR_H

#include "typedef.h"

typedef struct _BT15_REPAIR_API {
    unsigned int (*need_buf)(short max_packet_len);
    void (*open)(unsigned char *ptr, short max_packet_len);
    void (*run)(unsigned char *ptr, short *inbuf, short *oubuf, short len, short err_flag);
} BT15_REPAIR_API;
extern BT15_REPAIR_API *get_repair_api();

u32 LossPacketRepair_QueryBuffSize();
s8 LossPacketsRepair_init(void *pbuf);
void LossPacketsRepair_exit(void);
void LossPacketsRepair_run(s16 *inbuf, s16 *outbuf, u16 point, u8 repair_flag);

#endif

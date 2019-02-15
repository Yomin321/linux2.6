#ifndef NV_MEM_H
#define NV_MEM_H

#include "typedef.h"

#define NV_MEM_SIZE     256

#define REMOTE_DB_CNT   10
#define REMOTE_DB_SIZE  24

enum {
    /*can not change start*/
    NV_MEM_CRC       = 0,               //0 - 1 CRC16
    SYSCFG_REMOTE_DB = 2,               //2 - (SYSCFG_REMOTE_DB + REMOTE_DB_CNT * REMOTE_DB_SIZE-1)
    NV_MEM_SYS_VOL   = SYSCFG_REMOTE_DB + REMOTE_DB_CNT * REMOTE_DB_SIZE,
    /*can not change  end*/

    NV_MEM_MAX       =	NV_MEM_SIZE,
};

void nv_mem_init(void);

u8 nv_mem_set(u8 *ptr, u16 index, u16 size);

u8 nv_mem_get(u8 *ptr, u16 index, u16 size);

// void get_bt_database(void *ptr, u16 seek, u16 len);

// void set_bt_database(void *ptr, u16 seek, u16 len);

void nv_mem_test_demo(void);

#endif  //NV_MEM_H

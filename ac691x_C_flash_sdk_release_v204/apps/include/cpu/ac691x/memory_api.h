#ifndef _MEMORY_API_H_
#define _MEMORY_API_H_

#include "typedef.h"
#include "common/common.h"


u8 bt_vol_get_memory(u8 *ptr, u16 index, u16 size);

void bt_vol_set_memory(u8 *ptr, u16 index, u16 size);

void memory_init(u32 parm);

u8 iic_name_get_memory(u8 *ptr, u16 index, u16 size);
u8 iic_name_get_memory(u8 *ptr, u16 index, u16 size);
void get_bt_database(void *ptr, u16 seek, u16 len);
void set_bt_database(void *ptr, u16 seek, u16 len);
int set_bt_database_vm(u8 vm_remote_db, void *buf, int offset, int len);
int get_bt_database_vm(u8 vm_remote_db, void *buf, int offset, int len);


#endif

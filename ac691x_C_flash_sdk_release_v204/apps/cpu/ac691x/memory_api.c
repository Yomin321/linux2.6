/*--------------------------------------------------------------------------*/
/**@file     iic_io.c
   @brief    IO模拟的IIC的驱动
   @details
   @author  zhiying
   @date   2013-3-26
   @note
*/
/*----------------------------------------------------------------------------*/
#include "memory_api.h"
#include "iic.h"
#include "sdk_cfg.h"
#include "uart.h"
#include "nv_mem.h"
#include "flash_api.h"


enum {
    MEM_NON = 0x00,
    MEM_EEPROM,
    MEM_VM_FLASH,
    MEM_MV_RAM
};

static u8 memory_flag = MEM_NON;

/*----------------------------------------------------------------------------*/

/*
 * eeprom_init
 *
 */
void memory_init(u32 parm)
{
#if EEPROM_EN
    iic_init();
    if (eeprom_verify()) {
        puts("mem == eeprom\n");
        memory_flag = MEM_EEPROM;
        return;
    }
#endif

#if VM_MEM_EN
//    if (flash_init_api(parm) == 0) {	//0 : err non
    if (TRUE == vm_init_check()) {
        puts("mem == vm\n");
        memory_flag = MEM_VM_FLASH;
        return;
    }
#endif

#if NV_MEM_EN
    nv_mem_init();
    puts("men == nv\n");
    memory_flag = MEM_MV_RAM;
    return;
#endif
}


/*
 *void bt_vol_get_memory(void)
 *
 * */
u8 bt_vol_get_memory(u8 *ptr, u16 index, u16 size)
{
    u8 bt_vol_val;
    if (MEM_EEPROM == memory_flag) {
#if EEPROM_EN
        bt_vol_val = get_memory(BT_VOL_ADD);
        otp_printf("bt_vol_val1 : %d\n", bt_vol_val);
        return bt_vol_val;
#endif
    } else if (MEM_VM_FLASH == memory_flag) {
#if VM_MEM_EN
        vm_read(VM_SYS_VOL, ptr, VM_SYS_VOL_LEN);
        return *ptr;
#endif
    } else if (MEM_MV_RAM == memory_flag) {
#if NV_MEM_EN
        nv_mem_get(ptr, index, size);
        /* printf("nv_vol:%d\n", dac_ctl.sys_vol_l); */
        ///limit
        return *ptr;
#endif
    }
    return 0;
}


/*
 *void bt_vol_set_memory(void)
 *
 */
void bt_vol_set_memory(u8 *ptr, u16 index, u16 size)
{
    if (MEM_EEPROM == memory_flag) {
#if EEPROM_EN
        set_memory(BT_VOL_ADD, *ptr);
#endif
    } else if (MEM_VM_FLASH == memory_flag) {
#if VM_MEM_EN
        vm_cache_write(VM_SYS_VOL, ptr, VM_SYS_VOL_LEN);		//4 ~ 2s
#endif
    } else if (MEM_MV_RAM == memory_flag) {
#if NV_MEM_EN
        nv_mem_set(ptr, index, size);
#endif
    }
}
/*
 *u8 iic_name_get_memory(u8 *ptr,u16 index,u16 size)
 *
 * */
u8 iic_name_get_memory(u8 *ptr, u16 index, u16 size)
{
    u8 i;
    u8 tmp_index;

    tmp_index = (u8)index;

    if (index > NV_MEM_SIZE || (index + size > NV_MEM_SIZE)) {
        otp_printf("eeprom_get err!\n");
        return 0;
    }

    otp_printf("tmp_index1 %d,size :%d\n", tmp_index, size);
    for (i = 0; i < size; i++) {
        ptr[i] = read_eerom((u8)tmp_index);
        tmp_index++;
    }
    put_buf(ptr, size);

    return 1;
}

/*
 *
 *u8 iic_name_get_memory(u8 *ptr,u16 index,u16 size)
 *
 *  */

u8 iic_name_set_memory(u8 *ptr, u16 index, u16 size)
{
    u8 i;
    u8 tmp_index;

    tmp_index = (u8)index;
    otp_printf("tmp_index2 %d,size :%d\n", tmp_index, size);
    if (index > NV_MEM_SIZE || (index + size > NV_MEM_SIZE)) {
        otp_printf("eeprom_mem_set err!\n");
        return 0;
    }

    put_buf(ptr, size);
    for (i = 0; i < size; i++) {
        write_eerom((u8)tmp_index, ptr[i]);
        tmp_index++;
    }
    puts("set\n");
#if 0
    tmp_index = (u8)index;
    for (i = 0; i < size;) {
        ptr[i] = read_eerom((u8)tmp_index);
        /* put_buf(ptr,size); */
        /* printf("%02x ", ptr[i]); */
        tmp_index++;
        i++;
        if (i % 16 == 0) {
            printf("\n");
        }
    }
    put_buf(ptr, size);
    printf("\n");
#endif

    return 1;
}



void get_bt_database(void *ptr, u16 seek, u16 len)
{
    if (seek + len > REMOTE_DB_CNT * REMOTE_DB_SIZE) {
        otp_printf("get_bt_database err!\n");
    }
    /* otp_printf("get_bt_database!\n"); */
    if (MEM_EEPROM == memory_flag) {
#if EEPROM_EN
        iic_name_get_memory((u8 *)ptr, SYSCFG_REMOTE_DB + seek, len);
#endif
    } else if (MEM_VM_FLASH == memory_flag) {
#if VM_MEM_EN
        get_bt_database_vm(VM_REMOTE_DB, ptr, seek, len);
#endif
    } else if (MEM_MV_RAM == memory_flag) {
#if NV_MEM_EN
        nv_mem_get((u8 *)ptr, SYSCFG_REMOTE_DB + seek, len);
#endif
    }
}

void set_bt_database(void *ptr, u16 seek, u16 len)
{
    if (seek + len > REMOTE_DB_CNT * REMOTE_DB_SIZE) {
        printf("set_bt_database err!\n");
    }
    /* otp_printf("set_bt_database!\n"); */
    if (MEM_EEPROM == memory_flag) {
#if EEPROM_EN
        iic_name_set_memory((u8 *)ptr, SYSCFG_REMOTE_DB + seek, len);
#endif
    } else if (MEM_VM_FLASH == memory_flag) {
#if VM_MEM_EN
        set_bt_database_vm(VM_REMOTE_DB, ptr, seek, len);
#endif
    } else if (MEM_MV_RAM == memory_flag) {
#if NV_MEM_EN
        nv_mem_set((u8 *)ptr, SYSCFG_REMOTE_DB + seek, len);
#endif
    }
}


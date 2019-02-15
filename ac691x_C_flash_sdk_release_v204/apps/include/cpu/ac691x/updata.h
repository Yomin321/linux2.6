#ifndef _UPDATA_H_
#define _UPDATA_H_

#include "typedef.h"

//updata_flag
#define UPDATA_FLAG_ADDR		((void *)(0x14000-0x80))		/* (0x14000-0x80)-0x14000:		reserved_ram for updata */

#define UPDATA_MAGIC			(0x5A00)		//防止CRC == 0 的情况

typedef enum {
    UPDATA_NON = UPDATA_MAGIC,
    UPDATA_READY,
    UPDATA_SUCC,
    UPDATA_PARM_ERR,
    UPDATA_DEV_ERR,
    UPDATA_KEY_ERR,
} UPDATA_RESULT;

typedef enum {
    USB_UPDATA = UPDATA_MAGIC,		//0x5A00
    SD0_UPDATA,						//0x5A01
    SD1_UPDATA,
    PC_UPDATA,
    UART_UPDATA,
    BT_UPDATA,
    // BLE_UPDATA,

    NON_DEV = 0xFFFF,
} UPDATA_TYPE;

typedef struct _UPDATA_PARM {
    u16 parm_crc;
    u16 parm_type;				//UPDATA_TYPE:sdk pass parm to uboot
    u16 parm_result;			//UPDATA_TYPE:uboot return result to sdk
    u8  file_patch[32];			//updata file patch
    u8 	parm_priv[32];			//sd updata
} UPDATA_PARM;

void updata_mode_api(UPDATA_TYPE type, ...);
void device_updata(void *dev);
u16 updata_result_get(u32 first_start_flag);
bool device_is_first_start();
void update_result_deal();
bool get_updata_flag();

#endif

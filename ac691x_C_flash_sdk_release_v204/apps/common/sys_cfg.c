#include "sys_cfg.h"
#include "sydf/syd_file.h"
#include "crc_api.h"
#include "string.h"
#include "uart.h"
#include "sdk_cfg.h"

#if 1
#define READ_CFG_16( buffer, pos) ( ((u16) buffer[pos]) | (((u16)buffer[pos+1]) << 8))
#define READ_CFG_32( buffer, pos) ( ((u32) buffer[pos]) | (((u32)buffer[pos+1]) << 8) | (((u32)buffer[pos+2]) << 16) | (((u32) buffer[pos+3])) << 24)
#else
#define READ_CFG_16( buffer, pos) ( ((u16) buffer[pos+1]) | (((u16)buffer[pos  ]) << 8))
#define READ_CFG_32( buffer, pos) ( ((u32) buffer[pos+3]) | (((u32)buffer[pos+2]) << 8) | (((u32)buffer[pos+1]) << 16) | (((u32) buffer[pos])) << 24)
#endif

//cfg
APP_USE_FLASH_SYS_CFG app_use_flash_cfg;
void parse_sys_cfg(u32 cfg_addr)
{
#ifdef FLASH_MODE
    FLASH_SYS_CFG sys_cfg_val;
    FLASH_SYS_CFG *sys_cfg = &sys_cfg_val;

    u8 cfg_data[16];
    u8 *cfg_ptr;
    u32 i;

    /* printf("cfg_addr =  %08x \n", cfg_addr); */
    memcpy(sys_cfg, (void *)(FLASH_BASE_ADDR + cfg_addr), sizeof(FLASH_SYS_CFG));
    app_use_flash_cfg.cfg_zone_addr = sys_cfg->flash_cfg.cfg_zone_addr;
    app_use_flash_cfg.flash_size = sys_cfg->flash_cfg.flash_size;
    app_use_flash_cfg.cfg_zone_size = sys_cfg->flash_cfg.cfg_zone_size;

    /* printf("cfg_zone_addr =%08x \n", app_use_flash_cfg.cfg_zone_addr); */
    /* printf("cfg_zone_size =%08x \n", app_use_flash_cfg.cfg_zone_size); */

    cfg_ptr = (u8 *)(FLASH_BASE_ADDR + app_use_flash_cfg.cfg_zone_addr);
    for (i = 0; i < app_use_flash_cfg.cfg_zone_size; i += 16) {
        u32 tmp;
        memcpy(cfg_data, cfg_ptr, 16);
        /* printf_buf(cfg_data, 16); */
        if (memcmp((const char *)cfg_data, "BTIF", 4) == 0) {
            app_use_flash_cfg.btif_addr = READ_CFG_32(cfg_data, 4);
            app_use_flash_cfg.btif_len = READ_CFG_32(cfg_data, 8);
            otp_printf("btif_addr =%08x \n", app_use_flash_cfg.btif_addr);
            otp_printf("btif_len =%08x \n", app_use_flash_cfg.btif_len);
        } else if (memcmp((const char *)cfg_data, "VMIF", 4) == 0) {
            app_use_flash_cfg.vmif_addr = READ_CFG_32(cfg_data, 4);
            app_use_flash_cfg.vmif_len = READ_CFG_32(cfg_data, 8);
            otp_printf("VM_addr =%08x \n", app_use_flash_cfg.vmif_addr);
            otp_printf("VM_len =%08x \n", app_use_flash_cfg.vmif_len);
        } else {
            tmp = READ_CFG_32(cfg_data, 4);
            otp_printf("USER_addr =%08x \n", tmp);
            tmp = READ_CFG_32(cfg_data, 8);
            otp_printf("USER_len =%08x \n", tmp);
        }
        cfg_ptr += 16;
    }
#endif
}

extern u32 jump_addr_base;
extern u32 code_begin_addr;
extern u32 pwr_info_base;
extern u32 low_pwr_base;
u32 *cfg_info_addr[CFG_ADR_MAX];

#ifdef FLASH_MODE
FLASH_SYS_CFG sys_cfg;
FLASHHEADER syd_head;
FLASHFILEHEAD syd_member_head;
#endif

void cfg_info_init(u32 app_addr, u32 cfg_addr)
{
    u32 code_offset = app_addr;
    u32 *jump_base;
    u32 otp_offset = 0;
    u8 idx;

#ifdef FLASH_MODE
    puts("\n=========FlashMode=============\n");

    u32 sys_cfg_addr = cfg_addr;
    u16 sdfile_filenum;
    u32 ac691x_cfg_len;
    u32 ac691x_cfg_addr = 0;/*map addr*/
    u32 syd_head_base;		/*map addr*/
    u32 syd_member_base;	/*map addr*/
    u32 cfg_jump_addr;
    u32 btif_base;			/*map addr*/
    otp_printf("sys_cfg_addr:0x%x\n", sys_cfg_addr);
    otp_printf("code_offset:0x%x   %x %d\n", code_offset, &sys_cfg, sizeof(FLASH_SYS_CFG));

    memcpy(&sys_cfg, (void *)(FLASH_BASE_ADDR + sys_cfg_addr), sizeof(FLASH_SYS_CFG));
    syd_head_base = sys_cfg.flash_cfg.sdfile_head_addr + FLASH_BASE_ADDR;
    syd_member_base = syd_head_base + sizeof(FLASHHEADER);

    memcpy(&syd_head, (u8 *)syd_head_base, sizeof(FLASHHEADER));
    if (syd_head.crc16 == crc16(&syd_head.crcfileheaddata, sizeof(FLASHHEADER) - 2)) {
        otp_printf("TotalNum:0x%x\n", syd_head.filenum);
        for (u8 i = 0; i < syd_head.filenum; i++) {
            memcpy(&syd_member_head, (u8 *)(syd_member_base + i * sizeof(FLASHFILEHEAD)), sizeof(FLASHFILEHEAD));
            otp_printf("Name %d:%s\n", i, syd_member_head.name);
            if (memcmp(syd_member_head.name, "ac691x_cfg.bin", 14) == 0) {
                puts("ac691x_cfg finded\n");
                if (syd_member_head.crc16 == crc16((u8 *)(syd_member_head.addr + FLASH_BASE_ADDR), 800)) {
                    ac691x_cfg_addr = syd_member_head.addr + FLASH_BASE_ADDR;
                    ac691x_cfg_len = syd_member_head.len;
                    otp_printf("ac691x_cfg addr:0x%x\n", ac691x_cfg_addr);
                    otp_printf("ac691x_cfg len:%d\n", ac691x_cfg_len);
                    /* put_buf((u8*)ac691x_cfg_addr,800); */
                }
                break;
            }
        }
    } else {
        puts("sdfile_head crc ERROR\n");
    }

    /*BTIF Zone*/
    btif_base = app_use_flash_cfg.btif_addr + FLASH_BASE_ADDR - code_offset;
    cfg_jump_addr = btif_base + app_use_flash_cfg.btif_len - 128;
    /* put_buf((u8 *)(btif_base + app_use_flash_cfg.btif_len - 800), 800); */
    if (ac691x_cfg_addr) {
        puts("parse cfg zone:\n");
        for (idx = 0; idx < 4; idx++) {
            jump_base = (u32 *)(cfg_jump_addr + (4 * idx));
            otp_printf("cfg[%d]_addr:0x%04x\n", idx, *jump_base);
            if (*jump_base != 0xFFFFFFFF) {
                cfg_info_addr[idx] = (u32 *)(*jump_base + FLASH_BASE_ADDR - code_offset);
            } else {
                cfg_info_addr[idx] = NULL;
            }
        }
        cfg_info_addr[CFG_ADR_LOW_PWR] = (u32 *)(btif_base + app_use_flash_cfg.btif_len - 224);
        cfg_info_addr[CFG_ADR_PWR_INFO] = (u32 *)(btif_base + app_use_flash_cfg.btif_len - 144);
        for (idx = 0; idx < CFG_ADR_MAX; idx++) {
            otp_printf("cfg_info_addr[%d]:0x%04x\n", idx, cfg_info_addr[idx]);
        }
    }

#else

    printf("code_offset:0x%x\n", code_offset);
    printf("code_begin_addr:0x%x\n", &code_begin_addr);
    printf("jump_addr_base:0x%x\n", &jump_addr_base);

    if (&code_begin_addr >= 0x1FF0000) {
        puts("otp_mode\n");
        otp_offset = 0x200;
    } else {
        puts("flash_mode\n");
    }

    for (idx = 0; idx < 4; idx++) {
        jump_base = (u32 *)((u32)&jump_addr_base - code_offset + (4 * idx)); //addr_jump_tab[idx]
        if (*jump_base != 0xFFFFFFFF) {
            cfg_info_addr[idx] = (u32 *)(*jump_base + (u32)&code_begin_addr - code_offset - otp_offset);
        } else {
            cfg_info_addr[idx] = NULL;
        }
        //printf("jump_base%d:0x%x\t0x%x\n", idx, jump_base, *jump_base);
    }
    cfg_info_addr[CFG_ADR_LOW_PWR] = (u32 *)((u32)&low_pwr_base - code_offset);
    cfg_info_addr[CFG_ADR_PWR_INFO] = (u32 *)((u32)&pwr_info_base - code_offset);
    for (idx = 0; idx < CFG_ADR_MAX; idx++) {
        printf("cfg_info_addr[%d]:0x%04x\n", idx, cfg_info_addr[idx]);
    }
#endif
}

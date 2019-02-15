#include "nv_mem.h"
#include "hw_cpu.h"
#include "rtc_api.h"
#include "crc_api.h"
#include "uart.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".rtc_app_bss")
#pragma data_seg(	".rtc_app_data")
#pragma const_seg(	".rtc_app_const")
#pragma code_seg(	".rtc_app_code")
#endif

/* #define NV_MEM_DBG */

#ifdef  NV_MEM_DBG
#define nv_mem_putchar        putchar
#define nv_mem_printf         otp_printf
#define nv_mem_buf            printf_buf
#else
#define nv_mem_putchar(...)
#define nv_mem_printf(...)
#define nv_mem_buf(...)
#endif    //NV_MEM_DBG

#define P33_CS_H            JL_POWER_DOWN->P33_CON  |= BIT(0)
#define P33_CS_L            JL_POWER_DOWN->P33_CON  &= ~BIT(0)

static u8 P33_BUF(u8 buf)
{
    JL_POWER_DOWN->P33_DAT = buf;
    JL_POWER_DOWN->P33_CON |= BIT(5);

    while (JL_POWER_DOWN->P33_CON & BIT(1));
    return JL_POWER_DOWN->P33_DAT;
}

static void nv_p33_tx_1byte(u16 addr, u8 data0)
{
    OS_ENTER_CRITICAL();
    P33_CS_H;

    addr = addr + 0x3f00;

    P33_BUF(((addr > 0x3f) << 6) | (addr & 0x3f));                      //wr    //adr 3

    if (addr > 0x3f) {
        P33_BUF(addr >> 6);    //wr    //adr 3
    }

    P33_BUF(data0);

    P33_CS_L;
    OS_EXIT_CRITICAL();
}

static u8 nv_p33_rx_1byte(u16 addr)
{
    u8 data;
    OS_ENTER_CRITICAL();

    P33_CS_H;

    addr = addr + 0x3f00;

    P33_BUF(BIT(7) | ((addr > 0x3f) << 6) | (addr & 0x3f));             //rd    //adr 3

    if (addr > 0x3f) {
        P33_BUF(addr >> 6);    //wr    //adr 3
    }

    data = P33_BUF(0x5e);

    P33_CS_L;

    OS_EXIT_CRITICAL();
    return data;
}

static u8 nv_mem_crc_check(void)
{
    u16 crc, crc1;
    u8 *p;
    u16 i;

    //get crc
    p = (u8 *)(&crc);
    p[0] = nv_p33_rx_1byte(0);
    p[1] = nv_p33_rx_1byte(1);
    nv_mem_printf("crc:%x\n", crc);

    //calculate crc
    u8 nv_mem[NV_MEM_SIZE - 2];
    for (i = 0; i < NV_MEM_SIZE - 2; i++) {
        nv_mem[i] = nv_p33_rx_1byte(i + 2);
    }
    crc1 = crc16(nv_mem, NV_MEM_SIZE - 2);
    nv_mem_printf("crc1:%x\n", crc1);

    //compare crc
    if (crc == crc1 && crc != 0) {
        return 1;
    } else {
        return 0;
    }
}

static void nv_mem_crc_update(void)
{
    u16 i;
    u16 crc;
    u8 nv_mem[NV_MEM_SIZE - 2];

    for (i = 0; i < NV_MEM_SIZE - 2; i++) {
        nv_mem[i] = nv_p33_rx_1byte(i + 2);
    }

    crc = crc16(nv_mem, NV_MEM_SIZE - 2);
    nv_mem_printf("crc2:%x\n", crc);

    u8 *p = (u8 *)(&crc);
    nv_p33_tx_1byte(0, p[0]);
    nv_p33_tx_1byte(1, p[1]);
}

static u8 get_nv_mem_powerup_flag(void)
{
    if (nv_mem_crc_check()) {
        return 0;
    }
    return 1;
}

QLZ(.qlz_init)
void nv_mem_init(void)
{
    u16 i;

    if (get_nv_mem_powerup_flag()) {
        nv_mem_printf("nv mem reset\n");
        for (i = 2; i < NV_MEM_SIZE ; i++) {
            nv_p33_tx_1byte(i, 0xFF);
        }

        nv_mem_crc_update();
    }
}

u8 nv_mem_set(u8 *ptr, u16 index, u16 size)
{
    u16 i;
    u16 tmp_index;

    tmp_index = index;

    if (index > NV_MEM_SIZE || (index + size > NV_MEM_SIZE)) {
        nv_mem_printf("nv_mem_set err!\n");
        return 0;
    }

    for (i = 0; i < size; i++) {
        nv_p33_tx_1byte(tmp_index, ptr[i]);
        tmp_index++;
    }

    nv_mem_crc_update();

#if 0
    for (i = 0; i < NV_MEM_SIZE; i++) {
        nv_mem_printf("%02d:%02x ", i, nv_p33_rx_1byte(i));
    }
    nv_mem_printf("\n");
#endif

    return 1;
}

u8 nv_mem_get(u8 *ptr, u16 index, u16 size)
{
    u16 i;
    u16 tmp_index;

    tmp_index = index;

    if (index > NV_MEM_SIZE || (index + size > NV_MEM_SIZE)) {
        nv_mem_printf("nv_mem_get err!\n");
        return 0;
    }

    for (i = 0; i < size; i++) {
        ptr[i] = nv_p33_rx_1byte(tmp_index);
        tmp_index++;
    }

    return 1;
}



void nv_mem_test_demo(void)
{
#if 0
    u16 i;

    u8 nv_mem[NV_MEM_SIZE];

    for (i = 0; i < NV_MEM_SIZE; i++) {
        nv_mem[i] = nv_p33_rx_1byte(i);
    }

    printf_buf(nv_mem, NV_MEM_SIZE);

    for (i = 2; i < NV_MEM_SIZE; i++) {
        nv_p33_tx_1byte(i, i);
    }
    nv_mem_crc_update();

    for (i = 0; i < NV_MEM_SIZE; i++) {
        nv_mem[i] = nv_p33_rx_1byte(i);
    }

    printf_buf(nv_mem, NV_MEM_SIZE);

    extern void close_wdt(void);
    close_wdt();
    while (1);
#endif
}

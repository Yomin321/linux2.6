#include "compress.h"
#include "common/quicklz.h"
#include "uart.h"

#define QLZ_EN		1


extern u8 decompressed_dict_begin;
extern u8 data_begin;
extern u8 data_size;

#if 0
#define qlz_printf	otp_printf
#else
#define qlz_printf(...)
#endif

static void mem_cpy(void *dst, const void *src, u32 len)
{
    char *d = (char *)dst;
    const char *s = (const char *)src;
    for (u32 i = 0; i < len; i++) {
        d[i] = s[i];
    }
}
/*prevent qlz_size been optimized*/
void qlz_size(void) __attribute__((weak));
void bank_switch(u32 bank_num)
{
    // static u32 last_bank_num = -1;
    // if (last_bank_num == bank_num) {
    //     return;
    // }
#if QLZ_EN
#if QLZ_DATA_EN
    u8 *bank_info_start = (u8 *)((u32)&data_begin);//compress data_sec
#else
    u8 *bank_info_start = (u8 *)((u32)&data_begin + (u32)&data_size); //no-compress data_sec
#endif

    qlz_printf("\n\nsize:0x%x\tbank:%d\n", qlz_size, bank_num);
    if ((qlz_size == 0) && (bank_num > 0)) {
        qlz_printf("qlz_size NULL,return\n");
        return;
    }

    //otp_printf("%x\n", bank_info_start);
    //otp_printf("bank_num = %d\n", bank_num);
    //otp_printf("%x, %x, %x\n", &data_begin, &data_size,&decompressed_dict_begin);

    u32 bank_header_ptr;// = (u32)bank_info_start + bank_info_start[bank_num];
    {
        u8 *p = bank_info_start + bank_num * 4;
        //otp_printf("p = %x\n", p);
        bank_header_ptr = ((u32)p[0]) | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
        //otp_printf("bank_header_ptr = %x\n", bank_header_ptr);
        bank_header_ptr += (u32)bank_info_start;
    }
    //otp_printf("bank_header_ptr = %x\n", bank_header_ptr);
    u32 bank_header;
    {
        u8 *p = (u8 *)bank_header_ptr;
        bank_header = ((u32)p[0]) | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
    }
    u32 decompress_addr;
    {
        u8 *p = (u8 *)(bank_header_ptr + 4);
        decompress_addr = ((u32)p[0]) | ((u32)p[1] << 8) | ((u32)p[2] << 16) | ((u32)p[3] << 24);
    }
    u32 bank_data_ptr = bank_header_ptr + 8;
    if (bank_header & (1u << 31)) {
        u32 len = (bank_header << 1) >> 1;
        qlz_printf("len = %x, bank_data_ptr = %x\n", len, bank_data_ptr);
        // not compressed
        mem_cpy((void *)decompress_addr, (const void *)bank_data_ptr, len);
    } else {
        // get dictionary ptr
        u32 dict = (u32)&decompressed_dict_begin + bank_header;
        qlz_printf("decompress %x, dict = %x\n", bank_data_ptr, dict);
        qlz_printf("decompress addr = %x\n", decompress_addr);
        qlz_decompress2((const char *)bank_data_ptr,
                        (const char *)dict,
                        (void *)decompress_addr);
        //memset((u8*)decompress_addr,0xAA,qlz_size);
        /* if(bank_num == 2) {
        	put_buf((u8 *)decompress_addr, 7596);
        } */
    }
    //last_bank_num = bank_num;
#endif
}

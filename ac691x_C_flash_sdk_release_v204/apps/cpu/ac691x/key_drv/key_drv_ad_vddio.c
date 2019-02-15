#include "key_drv/key_drv_ad.h"
#include "key_drv/key.h"
#include "clock_api.h"
#include "adc_api.h"
#include "timer.h"
/* #include "sys_detect.h" */
#include "rtc/rtc_api.h"
#include "clock.h"
#include "power_manage_api.h"


#if KEY_AD_VDDIO_EN

#define AD_KEY_IO_PAX   9

#define R_UP       220     //22K

#define ADC10_33   (0x3ffL)
#define ADC10_30   (0x3ffL*2200/(2200 + R_UP))     //220K
#define ADC10_27   (0x3ffL*1000/(1000 + R_UP))     //100K
#define ADC10_23   (0x3ffL*510 /(510  + R_UP))     //51K
#define ADC10_20   (0x3ffL*330 /(330  + R_UP))     //33K
#define ADC10_17   (0x3ffL*240 /(240  + R_UP))     //24K
#define ADC10_13   (0x3ffL*150 /(150  + R_UP))     //15K
#define ADC10_10   (0x3ffL*91  /(91   + R_UP))     //9.1K
#define ADC10_07   (0x3ffL*62  /(62   + R_UP))     //6.2K
#define ADC10_04   (0x3ffL*30  /(30   + R_UP))     //3K
#define ADC10_00   (0)

#define AD_NOKEY        ((ADC10_33 + ADC10_30)/2)
#define ADKEY1_0		((ADC10_30 + ADC10_27)/2)
#define ADKEY1_1		((ADC10_27 + ADC10_23)/2)
#define ADKEY1_2		((ADC10_23 + ADC10_20)/2)
#define ADKEY1_3		((ADC10_20 + ADC10_17)/2)
#define ADKEY1_4		((ADC10_17 + ADC10_13)/2)
#define ADKEY1_5		((ADC10_13 + ADC10_10)/2)
#define ADKEY1_6		((ADC10_10 + ADC10_07)/2)
#define ADKEY1_7		((ADC10_07 + ADC10_04)/2)
#define ADKEY1_8		((ADC10_04 + ADC10_00)/2)


const u16 ad_key_table[] = {
    ADKEY1_0, ADKEY1_1, ADKEY1_2, ADKEY1_3, ADKEY1_4,
    ADKEY1_5, ADKEY1_6, ADKEY1_7, ADKEY1_8
};

/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
static u8 key_filter(u8 key)
{
    static u8 used_key = NO_KEY;
    static u8 old_key;
    static u8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }

    return used_key;
}


/*----------------------------------------------------------------------------*/
/**@brief   ad按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ad_key0_init(void)
*/
/*----------------------------------------------------------------------------*/
void ad_key_init(void)
{
    s32 ret;
    key_puts("ad key init\n");

    JL_PORTA->PU  &= ~BIT(AD_KEY_IO_PAX);
    JL_PORTA->PD  &= ~BIT(AD_KEY_IO_PAX);
    JL_PORTA->DIR |=  BIT(AD_KEY_IO_PAX);
    JL_PORTA->DIE &= ~BIT(AD_KEY_IO_PAX);

    /* adc_init_api(ad_table[0], LSB_CLK, SYS_LVD_EN); */
}

/*----------------------------------------------------------------------------*/
/**@brief   获取ad按键值
   @param   void
   @param   void
   @return  key_number
   @note    tu8 get_adkey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 get_adkey_value(void)
{
    u8 key_number;
    u32 key_value;

    key_value = adc_value[R_AD_CH_KEY];
    if (key_value > AD_NOKEY) {
        return key_filter(NO_KEY);
    }

    for (key_number = 0; key_number < sizeof(ad_key_table) / sizeof(ad_key_table[0]); key_number++) {
        if (key_value > ad_key_table[key_number]) {
            break;
        }
    }
    // otp_printf("adkey_value:%d   key_num:0x%x\n", key_value * 33 / 0x3ff, key_number);
    return key_filter(key_number);
}



const key_interface_t key_ad_info = {
    .key_type = KEY_TYPE_AD,
    .key_init = ad_key_init,
    .key_get_value = get_adkey_value,
};
#endif

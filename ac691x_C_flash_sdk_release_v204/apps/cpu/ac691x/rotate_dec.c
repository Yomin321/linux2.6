#include "rotate_dec.h"
#include "sdk_cfg.h"

void rotate_dec_set_clk(void)
{
    RDEC_SPD(12);//Tsr = (2^RDEC_SPD)/Flsb, (Tsr建议：0.5ms~2ms)
}

void rotate_dec_init(u8 mode)
{
    rotate_dec_set_clk();

    JL_PORTA->DIR |= (BIT(1) | BIT(2));
    JL_PORTA->PU |= (BIT(1) | BIT(2));
    JL_PORTA->PD &= ~(BIT(1) | BIT(2));

    RDEC_POL(mode);
    RDEC_EN(1);
    RDEC_CPND();
    otp_printf("JL_RDEC->CON = x%x\n", JL_RDEC->CON);

#if 0
    while (1) {
        s8 dat = rotate_dec_get_dat();
        if (dat) {
            otp_printf("dat = %d\n", dat);
        }
    }
#endif
}

s8 rotate_dec_get_dat(void)
{
    s8 ret = 0;
    if (RDEC_PND()) {
        ret = JL_RDEC->DAT;
        RDEC_CPND();
        otp_printf("dat = %d\n", ret);
    }
    return ret;
}



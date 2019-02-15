#include "clock.h"
#include "clock_api.h"
#include "bluetooth/avctp_user.h"
#include "bluetooth/bluetooth_api.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".clk_app_bss")
#pragma data_seg(	".clk_app_data")
#pragma const_seg(	".clk_app_const")
#pragma code_seg(	".clk_app_code")
#endif

#define DVDD_LEVEL(x)  SFR(JL_SYSTEM->LDO_CON0, 12, 3, x); \
	                       SFR(JL_SYSTEM->LDO_CON0, 21, 3, x);
void auto_match_dvdd_lev(u32 out_freq)
{
#if 0
    if (out_freq < 96000000L) {
        //set dvdd to 1.0v
        DVDD_LEVEL(6);   //dvdd 1.0v
    } else if (out_freq < 120000000L) {
        //set dvdd to 1.1v
        DVDD_LEVEL(5);   //dvdd 1.1v
    } else {
        //set dvdd to 1.2v
        DVDD_LEVEL(3);   //dvdd 1.2v
    }
#else
    DVDD_LEVEL(3);   //dvdd 1.2v
#endif
}


QLZ(.qlz_init)
void clock_init_app(SYS_CLOCK_INPUT sys_in, u32 input_freq, u32 out_freq)
{
    set_auto_dvdd_lev_cab(auto_match_dvdd_lev);
    clock_init(sys_in, input_freq, out_freq);

    init_t *init_func;

    list_for_each_clock_switcher(init_func) {
        (*init_func)();
    }
}

u32 cur_clk_bk = 0;

void auto_match_dvdd_lev1(u32 out_freq)
{
    cur_clk_bk = SYS_CLK;

    if (cur_clk_bk >= out_freq) { //降频率
        return;
    }

    auto_match_dvdd_lev(out_freq);
}

void auto_match_dvdd_lev2(u32 out_freq)
{
    if (cur_clk_bk <= out_freq) { //升频率
        return;
    }

    auto_match_dvdd_lev(out_freq);
}

AT(.common)
void set_sys_freq(u32 out_freq)
{
    /* if (out_freq == SYS_CLK) { */
    /* return; */
    /* } */
    auto_match_dvdd_lev1(out_freq);

    bt_close_eninv();

    clock_switching(out_freq);

    bt_open_eninv();

    auto_match_dvdd_lev2(out_freq);

    otp_printf("set_sys_freq = %d\n", out_freq);
}

AT(.common)
void set_apc_clk(u32 freq)
{
    //SFR(JL_CLOCK->CLK_CON1,6,2,1);//apc_src:pll_apc_clk
    //SFR(JL_CLOCK->CLK_CON2,18,2,0);//pll_apc_src: pll_192M
    if (freq == CALL_APC_Hz) {
        SFR(JL_CLOCK->CLK_CON2, 20, 4, 4); //192M/2=96M
    } else {
        SFR(JL_CLOCK->CLK_CON2, 20, 4, 8); //192M/4=48M
    }
}


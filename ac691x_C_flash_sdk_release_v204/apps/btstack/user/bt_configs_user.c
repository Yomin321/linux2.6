#include "bluetooth/avctp_user.h"
#include "task_bt.h"
#include "sdk_cfg.h"
#include "fcc_test.h"
#include "audio/ladc.h"
#include "audio/dac_api.h"
#include "audio/tone.h"
#include "aec_main.h"
#include "audio_improve_main.h"
#include "crc_api.h"
#include "power.h"
#include "aec_user.h"
#include "msg.h"
#include "audio/dac_api.h"
#include "audio/dac.h"
#include "sys_cfg.h"
#include "charge.h"

#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(	".bt_app_bss")
#pragma data_seg(	".bt_app_data")
#pragma const_seg(	".bt_app_const")
#pragma code_seg(	".bt_app_code")
#endif

//#define CFG_DEBUG_EN
#ifdef CFG_DEBUG_EN
#define cfg_puts 		puts
#define cfg_put_buf 	put_buf
#define cfg_printf		otp_printf
#else
#define cfg_puts(...)
#define cfg_put_buf(...)
#define cfg_printf(...)
#endif

static void bt_profile_select_init(void)
{
    u8 support_profile;
    support_profile = SPP_CH | HFP_CH | A2DP_CH | AVCTP_CH | HID_CH | AVDTP_CH;
#if (USER_SUPPORT_PROFILE_HFP==0)
    support_profile &= ~HFP_CH;
#endif
#if (USER_SUPPORT_PROFILE_AVCTP==0)
    support_profile &= ~AVCTP_CH;
#endif
#if (USER_SUPPORT_PROFILE_A2DP==0)
    support_profile &= ~A2DP_CH;
    support_profile &= ~AVCTP_CH;
    support_profile &= ~AVDTP_CH;
#endif
#if (USER_SUPPORT_PROFILE_SPP==0)
    support_profile &= ~SPP_CH;
#endif
#if (USER_SUPPORT_PROFILE_HID== 0)
    support_profile &= ~HID_CH;
#endif

    bt_cfg_default_init(support_profile);/*外部设置支持什么协议*/
}


#define BD_NAME_LEN_MAX		30
#define BD_NAME_NUM_MAX		10
#define BD_NAME_TOTAL		15
#if(BT_MODE == NORMAL_MODE)
static char host_name[BD_NAME_LEN_MAX] = "BR20_SDK";
#else
static char host_name[BD_NAME_LEN_MAX] = "BR20-RAM";
#endif

static void set_device_volume(int volume)
{
    dac_ctl.sys_vol_l = (volume *  get_max_sys_vol(0) / 127) ;
    dac_ctl.sys_vol_r = dac_ctl.sys_vol_l;
    set_sys_vol(dac_ctl.sys_vol_l, dac_ctl.sys_vol_r, FADE_OFF);
    otp_printf("phone:%d\tdac:%d\n", volume, dac_ctl.sys_vol_l);
}

static int get_dac_vol()
{
    return (dac_ctl.sys_vol_l * 127 / get_max_sys_vol(0)) ;
}
static void spp_data_deal(u8 packet_type, u16 channel, u8 *packet, u16 size)
{
    switch (packet_type) {
    case 1:
        /* puts("spp connect\n"); */
        break;
    case 2:
        /* puts("spp disconnect\n"); */
        break;
    case 7:
        /* puts("spp data\n"); */
#if AEC_DEBUG_ONLINE
        aec_config_online(packet, size);
#endif
        break;
    }
}

//u8 *bt_cfg = (u8*)(0x1000000+128*1024 - 0x62a0 - 256);
extern u32 args[3];
typedef struct _BT_ADDR {
    u16 crc;
    u8 data[6];
} bt_addr;
typedef struct _BT_NAME {
    u16 crc;
    u8 data[BD_NAME_LEN_MAX];
} bt_name;
typedef struct _BT_OSC_CAP {
    u16 crc;
    u8 cap_l;	/*internal cap left	*/
    u8 cap_r;	/*internal cap right*/
} bt_osc_cap;
struct _BT_SETUP {
    char tag[5];		/*BTCFG			*/
    u16 crc;			/*ver ~ reserve */
    u8 version;			/*0x00~0xFF		*/

    u8 total_num;		/*max = 10		*/
    u8 rf_power;		/*RF TxPower	*/
    u8 vol_max;			/*dac max vol	*/
    u8 cap_l;			/*default cap L	*/
    u8 cap_r;			/*default cap R	*/
    u8 charge_full;		/*charge voltage*/
    u8 vol_default;		/*power on vol	*/
    u8 reserve[5];		/*future use	*/
    bt_osc_cap osc_cap;	/*bt osc cap	*/

    bt_addr addr;		/*bd_addr     	*/
    bt_name name[0];	/*bd_name 		*/
} _GNU_PACKED_;
typedef struct _BT_SETUP BT_SETUP;


BT_SETUP *bt_setup;
extern u32 config_info_begin;
extern u8 get_pull_up_num();

/*
***********************************************************************************
*					BT SETUP INIT
*
*Description: This function is called to init bt config infomation
*
*Argument(s): addr:dst bd_addr
*			  name:dst bd_name
*             name_idx:host_name index(0~(max-1))
*Returns	: none
*
*Note(s)	: 1)more name,you can select one of them through name_idx
*			  2)single name,select the newest one
***********************************************************************************
*/
void bt_setup_init(u8 *addr, char *name, u8 name_idx, u8 *max_pwr)
{
    u8 update_idx = 0;
    u8 newest_idx = 0;
    u16 update_crc = 0;

    bt_setup = (BT_SETUP *)cfg_info_addr[CFG_ADR_BT_INFO];
    if (bt_setup == NULL) {
        puts("bt_setup NULL\n");
        return;
    }

    cfg_printf("BtCfg_Addr:0x%x\n", bt_setup);
    /* printf_buf((u8 *)bt_setup, 64); */

    if (memcmp(bt_setup->tag, "BTCFG", 5) == 0) {
        if (bt_setup->crc == crc16(&bt_setup->version, 13)) {
            cfg_printf("version:%d\n", bt_setup->version);
            cfg_printf("rf_power:%d\n", bt_setup->rf_power);
            cfg_printf("vol_max:%d\n", bt_setup->vol_max);
            cfg_printf("vol_default:%d\n", bt_setup->vol_default);
            cfg_printf("osc_L:0x%x\n", bt_setup->cap_l);
            cfg_printf("osc_R:0x%x\n", bt_setup->cap_r);
            cfg_printf("trim osc_L:0x%x\n", bt_setup->osc_cap.cap_l);
            cfg_printf("trim osc_R:0x%x\n", bt_setup->osc_cap.cap_r);
            otp_printf("charge_full:%d\n", bt_setup->charge_full);
            *max_pwr = bt_setup->rf_power;
        }

        /* check bd_addr */
        if (bt_setup->addr.crc == crc16(bt_setup->addr.data, 6)) {
            memcpy(addr, bt_setup->addr.data, 6);
            cfg_puts("cfg_addr OK:");
            cfg_put_buf(addr, 6);
        }

        /* check update_info */
        update_idx = bt_setup->total_num;/*update info base*/
        while (update_idx < BD_NAME_TOTAL) {
            cfg_printf("\nupdate_idx:%d", update_idx);
            cfg_put_buf((u8 *)&bt_setup->name[update_idx], BD_NAME_LEN_MAX + 2);
            update_crc = crc16(bt_setup->name[update_idx].data, BD_NAME_LEN_MAX);
            if ((bt_setup->name[update_idx].crc == update_crc) && (update_crc != 0)) {
                newest_idx = update_idx;
            }
            update_idx++;
        }

        /* update bd_name */
        if (newest_idx >= bt_setup->total_num) {
            /* update config */
            memcpy(name, bt_setup->name[newest_idx].data, BD_NAME_LEN_MAX);
            cfg_printf("update_name%d:%s\n", newest_idx, name);
        } else {
            /* default config */
            if (bt_setup->total_num > 1) {
                if (name_idx >= bt_setup->total_num) {
                    name_idx = 0;
                }
                if (bt_setup->name[name_idx].crc == crc16(bt_setup->name[name_idx].data, BD_NAME_LEN_MAX)) {
                    memcpy(name, bt_setup->name[name_idx].data, BD_NAME_LEN_MAX);
                    otp_printf("more_name%d OK:%s\n", name_idx, name);
                }
            } else {
                memcpy(name, bt_setup->name[0].data, BD_NAME_LEN_MAX);
                otp_printf("one_name:%s\n", name);
            }
        }
    } else {
        puts("BT setup info invalid\n\n");
    }
}

void get_cfg_vol_max(u8 *sys_vol_max, u8 *sys_vol_default)
{
    *sys_vol_max = MAX_SYS_VOL_L;
    *sys_vol_default = SYS_DEFAULT_VOL;
    bt_setup = (BT_SETUP *)cfg_info_addr[CFG_ADR_BT_INFO];
    if (bt_setup == NULL) {
        puts("bt_setup NULL\n");
        return;
    }

    if (memcmp(bt_setup->tag, "BTCFG", 5) == 0) {
        if (bt_setup->crc == crc16(&bt_setup->version, 13)) {
            otp_printf("vol_max:%d\n", bt_setup->vol_max);
            otp_printf("vol_default:%d\n", bt_setup->vol_default);
            if (bt_setup->vol_max < 31) {
                *sys_vol_max = bt_setup->vol_max;
            }
            if (bt_setup->vol_default < 31) {
                *sys_vol_default = bt_setup->vol_default;
            }
        }
    }
}
u16 get_cfg_charge_full(void)
{
    u16 charge_full = POWEROFF_THRESHOLD_VALUE;
    bt_setup = (BT_SETUP *)cfg_info_addr[CFG_ADR_BT_INFO];
    if (bt_setup == NULL) {
        puts("bt_setup NULL\n");
        return charge_full;
    }

    if (memcmp(bt_setup->tag, "BTCFG", 5) == 0) {
        if (bt_setup->crc == crc16(&bt_setup->version, 13)) {
            otp_printf("charge_full:%d\n", bt_setup->charge_full);
            if (bt_setup->charge_full >= 15 && bt_setup->charge_full <= 70) {
                charge_full = bt_setup->charge_full * 10;
            }
        }
    }
    return charge_full;
}

/*
***********************************************************************************
*					OSC SETUP INIT
*
*Description: This function is called to init internal OSC
*
*Argument(s): none

*Returns	: none
*
*Note(s)	: 1)
***********************************************************************************
*/
void bt_osc_init(void)
{
    if (memcmp(bt_setup->tag, "BTCFG", 5) == 0) {
        if (bt_setup->osc_cap.crc == crc16((void *)&bt_setup->osc_cap.cap_l, 2)) {
            otp_printf("trim_cap l=0x%x, r=0x%x\n", bt_setup->osc_cap.cap_l, bt_setup->osc_cap.cap_r);
            bt_osc_internal_cfg(bt_setup->osc_cap.cap_l, bt_setup->osc_cap.cap_r);
            return;
        } else if (bt_setup->crc == crc16(&bt_setup->version, 13)) {
            otp_printf("cfg_cap l=0x%x, r=0x%x\n", bt_setup->cap_l, bt_setup->cap_r);
            bt_osc_internal_cfg(bt_setup->cap_l, bt_setup->cap_r);
            return;
        }
    }
    otp_printf("default_cap l=0x%x, r=0x%x\n", BT_OSC_INTERNAL_L, BT_OSC_INTERNAL_R);
    bt_osc_internal_cfg(BT_OSC_INTERNAL_L, BT_OSC_INTERNAL_R);
}

QLZ(.qlz_init)
u8 max_pwr = 0xff;
void bt_function_select_init()
{
    /*蓝牙功能流程选择配置*/
#if(BT_MODE==NORMAL_MODE)
    u8 debug_addr[6] = {0x89, 0x89, 0x78, 0x69, 0x90, 0x48};
    bt_setup_init(debug_addr, (char *)host_name, get_pull_up_num(), &max_pwr);
    aec_cfg_init();
    __set_max_pwr(BT_DISCONNECT_MAX_PWR);
#else
    u8 debug_addr[6] = {0x11, 0x22, 0x33, 0x33, 0x22, 0x11};
    u8 unuse_addr[6];
    char unuse_name[BD_NAME_LEN_MAX];
    bt_setup_init(unuse_addr, (char *)unuse_name, get_pull_up_num(), &max_pwr);
    __set_max_pwr(max_pwr);
#endif


    __set_host_name(host_name, sizeof(host_name));
    __set_pin_code("0000");
    __set_bt_mac_addr(debug_addr);
    bt_profile_select_init();

    __set_user_ctrl_conn_num(BT_CONNTCT_NUM);     /*用户设置支持连接的个数，1 或 2*/
    __set_auto_conn_device_num(BT_CONNTCT_NUM);   /*该接口用于设置上电回连需要依次搜索设备的个数。0表示上电不回连。大于20无效，直到连上一个*/

    __bt_set_sniff(SNIFF_MODE_CONF);  /*设置进入sniff是进入poweroff还是powerdown*/
    __bt_set_update_battery_time(10000); /*设置电量显示发送更新的周期时间，为0表示关闭电量显示功能，单位毫秒，u32, 不能小于5000ms*/
    __bt_set_a2dp_auto_play_flag(0); /*高级音频连接完成自动播放歌曲使能, 0不使能，1使能*/
    __set_simple_pair_flag(1);       /*提供接口外部设置配对方式,1使能简易配对。0使用pin code, 会使用配置文件的值*/
    __set_sbc_cap_bitpool(53);
    __set_page_timeout_value(8000); /*回连搜索时间长度设置,可使用该函数注册使用，ms单位,u16*/
    __set_super_timeout_value(8000); /*回连时超时参数设置。ms单位。做主机有效*/

    __set_music_break_in_flag(1);  /* 音频抢断开关，0：不抢断，1：抢断*/
    __set_auto_pause_flag(0);
    __set_hfp_switch(1);             /*通话抢断开关，0：不抢断，1：抢断*/

    __set_esco_packet_type(1);
    __set_w_sco_slot(6, 4);
    __set_device_role(S_DEVICE_ROLE_SLAVE | M_DEVICE_ROLE_MASTER_TO_SLAVE); /*设置设备role*/

}
/*
蓝牙库用到的一系列可设置参数配置，通过handle的形式。
这样用户就不用考虑调用配置函数的具体位置。需要改变可以注册，不需要设置有默认参数
*/
static u8 fast_test_flag = 0;		//1A:enter fast test

u8 is_enter_fast_test(void)
{
    return (fast_test_flag == 0x1A);
}

void bt_fast_test_api(void)
{
    otp_printf("---bt_fast_test_api---\n");
    fast_test_flag = 0x1A;
    tone_toggle(1);//enable key_tone

    //不用宏包住
    dac_automute_init(0, -1, -1, -1); // 关自动mute
    dac_toggle(1);
    pa_umute();
    set_sys_vol(20, 20, FADE_OFF);
    microphone_open(5, 0);
}

//u32 parm
void bt_updata_start_api(u32 parm)
{
    task_post_msg(NULL, 1, MSG_BT_UPDATA_START);
}

void bt_updata_end_api(u32 parm)
{

    u8 updata_type = (u8)((parm >> 8) & 0xFF);
    u8 updata_ret  = (u8)(parm & 0xFF);

    printf("updata_type ret = %d\n", updata_type);
    printf("updata_ret  ret = %d\n", updata_ret);


    if (updata_type == 0x02) {		//updata_name
        if (!updata_ret) {
            task_post_msg(NULL, 1, MSG_BT_UPDATA_END);
        }
    } else if (updata_type == 0x01) {
        //updata_osc
    }
}

bool check_connect_hid(void)
{
#if (USER_SUPPORT_PROFILE_HID==1)
    u8 curr_channel = 0;
    static u8 check_hid_cnt = 0;
    curr_channel = get_curr_channel_state();
    if ((curr_channel & A2DP_CH) && (curr_channel & HFP_CH) && !(curr_channel & HID_CH) && get_call_status() == BT_CALL_HANGUP) {
        if (check_hid_cnt == 0) {
            check_hid_cnt = 6;
            return TRUE;
        }
    }
    if (check_hid_cnt) {
        check_hid_cnt--;
    }
#endif
    return FALSE;
}
void read_name_handle(u8 *name)
{
    putchar('\n');
    puts((const char *)name);
    putchar('\n');
}
/*电量等级变化*/
extern u16 low_power_off_value ;
static int bt_get_battery_value()
{
    //只支持1-9直接的值，9个等级
    u16 battery_level;
    u16 battery_value = get_battery_level() / 10;
    if (battery_value <= (low_power_off_value / 10)) {
        battery_level = 1;
    } else {
        battery_level = (battery_value - (low_power_off_value / 10));
    }
    /* printf("battery_level=0x%x\n",battery_level ); */
    if ((low_power_off_value >= 330) && (battery_level >= 4)) {

        battery_level += (low_power_off_value / 10 - 31);
    } else if ((low_power_off_value >= 330) && (battery_level >= 2)) {
        battery_level += (low_power_off_value / 10 - 32);
    }
    return battery_level;
}
extern void *a2dp_media_play(void *priv, const char *format,
                             u16(*read_handle)(void *priv, u8 *buf, u16 len),
                             bool (*seek_handle)(void *priv, u8 type, u32 offsiz));
extern void a2dp_media_stop(void **hdl);
extern int btstack_status_update_deal(u8 *ptr, u16 len);
extern void get_siri_status_handle_register(void (*handle)(int flag));
void siri_status_feedback(int flag)
{
    otp_printf("siri sta %d\n", flag);
}
u8 user_inf_buf[50];
QLZ(.qlz_init)
void bredr_handle_register()
{
    register_edr_init_handle();
    infor_2_user_handle_register(btstack_status_update_deal, user_inf_buf);
    discon_complete_handle_register(bt_discon_complete_handle);/*断开或者连接上会调用的函数，给客户反馈信息*/
#if (BT_MODE != NORMAL_MODE)
    fcc_test_handle_register(NULL, test_fcc_default_info); /*fcc测试函数注册*/
#endif
    spp_data_deal_handle_register(spp_data_deal);
    music_vol_change_handle_register(set_device_volume, get_dac_vol);
    bt_a2dp_audio_handle_register(a2dp_media_play, a2dp_media_stop);
    bt_phone_audio_handle_register(hook_sco_conn, hook_sco_disconn, hook_sco_rx);

    bt_fast_test_handle_register(bt_fast_test_api);
    bt_updata_handle_register(bt_updata_start_api, bt_updata_end_api);
    // read_remote_name_handle_register(read_name_handle);
    bt_noconn_pwr_handle_register(bt_noconn_pwr_down_in, bt_noconn_pwr_down_out);
    get_battery_value_register(bt_get_battery_value);   /*电量显示获取电量的接口*/
    get_siri_status_handle_register(siri_status_feedback);
}


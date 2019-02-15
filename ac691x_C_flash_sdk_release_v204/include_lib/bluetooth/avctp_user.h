#ifndef __AVCTP_USER_H__
#define __AVCTP_USER_H__


#include "typedef.h"


///***注意：该文件的枚举与库编译密切相关，主要是给用户提供调用所用。用户不能自己在中间添加值。*/
////----user (command) codes----////
typedef enum {
    /*
    使用user_send_cmd_prepare(USER_CMD_TYPE cmd,u16 param_len,u8 *param)发送命令
    //返回0表支持参数个数正确，返回1表不支持，2是参数错误
    要三个参数，没参数说明的命令参数param_len传0，param传NULL
    例子A、USER_CTRL_HFP_CALL_SET_VOLUME命令需要1个参数的使用例子：
    u8 vol = 8;
    user_send_cmd_prepare(USER_CTRL_HFP_CALL_SET_VOLUME,1, &vol);

    例子B、USER_CTRL_DIAL_NUMBER 参数要用数组先存起来，param_len是号码长度，param可传参数数组指针，
    user_val->income_phone_num已经存好号码
    user_send_cmd_prepare(USER_CTRL_DIAL_NUMBER,user_val->phone_num_len,user_val->income_phone_num);

    */

    //hfp链路部分
    //控制打电话音量，注意可能有些手机进度条有变化音量大小没变化，同步要设置样机DAC音量
    /*跟电话音量操作有关的操作最终都执行回调函数call_vol_change*/
    USER_CTRL_HFP_CALL_VOLUME_UP,       /*音量加1，手机可以同步显示*/
    USER_CTRL_HFP_CALL_VOLUME_DOWN,      /*音量减1，手机可以同步显示*/
    USER_CTRL_HFP_CALL_SET_VOLUME,   /*设置固定值，手机可以同步显示，需要传1个音量值*/
    USER_CTRL_HFP_CALL_GET_VOLUME,  /*获取音量，默认从call_vol_change返回*/

    //来电接听电话
    USER_CTRL_HFP_CALL_ANSWER,
    //挂断电话
    USER_CTRL_HFP_CALL_HANGUP,
    //回拨上一个打出电话
    USER_CTRL_HFP_CALL_LAST_NO,
    //获取当前通话电话号码
    USER_CTRL_HFP_CALL_CURRENT,
    //发送打电话时的信号选择DTMF tones ,有一个参数，参数支持{0-9, *, #, A, B, C, D}
    USER_CTRL_HFP_DTMF_TONES,
    //连接或断开SCO或esco,选择这个命令会自动判断要断开还是连接sco
    USER_CTRL_SCO_LINK,
    //连接SCO或esco
    USER_CTRL_CONN_SCO,
    //断开sco或esco
    USER_CTRL_DISCONN_SCO,
    //根据电话号码拨号
    /**USER_CTRL_DIAL_NUMBER命令有参数，参数要用数组先存起来，
    param_len是号码长度，param可传参数数组指针*/
    USER_CTRL_DIAL_NUMBER,
    //发送电量  /**要连接上HFP才有用*/
    USER_CTRL_SEND_BATTERY,
    //*控制siri状态*//*可以注册回调函数获取返回值*/
    USER_CTRL_HFP_GET_SIRI_STATUS,
    //*开启siri*/
    USER_CTRL_HFP_GET_SIRI_OPEN,
    //*关闭siri,一般说完话好像自动关闭了,如果要提前终止可调用*/
    USER_CTRL_HFP_GET_SIRI_CLOSE,
    //通话过程中根据提示输入控制
    /*
    例子
    char num = '1';
    user_send_cmd_prepare(USER_CTRL_SEND_DTMF_TONE,1,(u8 *)&num);
    */
    USER_CTRL_SEND_DTMF_TONE,

    //*两台手机通话互相切换*/
    USER_CTRL_HFP_CHANGE_STATE,
    //*挂断当前手机通话，转换到另外一台*/
    USER_CTRL_HFP_CHANGE_WORKING,

    //三方通话操作
    //应答
    USER_CTRL_HFP_THREE_WAY_ANSWER1,     //挂断当前去听另一个（未接听或者在保留状态都可以）
    USER_CTRL_HFP_THREE_WAY_ANSWER2,     //保留当前去接听, 或者用于两个通话的切换
    USER_CTRL_HFP_THREE_WAY_ANSWER1X,
    USER_CTRL_HFP_THREE_WAY_ANSWER2X,
    //拒听
    USER_CTRL_HFP_THREE_WAY_REJECT,           //拒绝后台来电
    USER_CTRL_HFP_BIA,

    //音乐控制部分
    //音乐播放
    USER_CTRL_AVCTP_OPID_PLAY,
    //音乐暂停
    USER_CTRL_AVCTP_OPID_PAUSE,
    //音乐停止
    USER_CTRL_AVCTP_OPID_STOP,
    //音乐下一首
    USER_CTRL_AVCTP_OPID_NEXT,
    //音乐上一首
    USER_CTRL_AVCTP_OPID_PREV,
    //音乐快进
    USER_CTRL_AVCTP_OPID_FORWARD,
    //音乐快退
    USER_CTRL_AVCTP_OPID_REWIND,
    //音乐循环模式
    USER_CTRL_AVCTP_OPID_REPEAT_MODE,
    USER_CTRL_AVCTP_OPID_SHUFFLE_MODE,
    //同步音量接口
    USER_CTRL_AVCTP_OPID_SEND_VOL,
    //链路操作部分
    //回连,使用的是VM的地址，一般按键操作不使用该接口
    USER_CTRL_START_CONNECTION,
    //通过地址去连接，如果知道地址想去连接使用该接口
    USER_CTRL_START_CONNEC_VIA_ADDR,
    //通过指定地址手动回连，该地址是最后一个断开设备的地址
    USER_CTRL_START_CONNEC_VIA_ADDR_MANUALLY,

    //断开连接，断开当前所有蓝牙连接
    USER_CTRL_DISCONNECTION_HCI,

    //读取远端名字
    USER_CTRL_READ_REMOTE_NAME,
    //有判断条件的，回连过程连接高级音频，避免手机连也自动发起连接，一般按键操作不使用该接口
    USER_CTRL_AUTO_CONN_A2DP,
    //连接高级音频，回来最后一个断开设备的地址
    USER_CTRL_CONN_A2DP,
    //断开高级音频，只断开高级音频链路，如果有电话还会保留
    USER_CTRL_DISCONN_A2DP,
    //断开SDP，一般按键操作不使用该接口
    USER_CTRL_DISCONN_SDP_MASTER,
    //蓝牙关闭
    USER_CTRL_POWER_OFF,
//    USER_CTRL_READ_SCAN_ENABLE       ,
    //关闭蓝牙可发现
    USER_CTRL_WRITE_SCAN_DISABLE,
    //打开蓝牙可发现
    USER_CTRL_WRITE_SCAN_ENABLE,
//   USER_CTRL_WRITE_SCAN_ENABLE_KEY   ,
    //关闭蓝牙可连接
    USER_CTRL_WRITE_CONN_DISABLE,
    //打开蓝牙可连接
    USER_CTRL_WRITE_CONN_ENABLE,
    //  USER_CTRL_WRITE_CONN_ENABLE_KEY     ,
    //控制蓝牙搜索，需要搜索附件设备做功能的连续说明情况在补充完善功能
    USER_CTRL_SEARCH_DEVICE,
    ///进入sniff模式，一般按键操作不使用该接口
    USER_CTRL_SNIFF_IN,
    USER_CTRL_SNIFF_EXIT,
    ///*hid操作定义*/
    //按键连接
    USER_CTRL_HID_CONN,
//    //只发一个按键，安卓手机使用
    USER_CTRL_HID_ANDROID,
    //只发一个按键，苹果和部分安卓手机适用
    USER_CTRL_HID_IOS,
//    //发两个拍照按键
    USER_CTRL_HID_BOTH,
    //HID断开
    USER_CTRL_HID_DISCONNECT,
//	//Home Key,apply to IOS and Android
//	USER_CTRL_HID_HOME				 ,
//	//Return Key,only support Android
//	USER_CTRL_HID_RETURN			 ,
//	//LeftArrow Key
//	USER_CTRL_HID_LEFTARROW			 ,
//	//RightArrow Key
//	USER_CTRL_HID_RIGHTARROW		 ,

//    //AVCTP断开，是音乐控制链路，一般不使用
    USER_CTRL_AVCTP_DISCONNECT,
//    //AVCTP连接，是音乐控制链路，一般不使用
    USER_CTRL_AVCTP_CONN,

    ///蓝牙串口发送命令
    /**USER_CTRL_SPP_SEND_DATA命令有参数，参数会先存起来，
    param_len是数据长度，param发送数据指针
    返回0,表示准备成功，会PENDing发完才返回
    3表示上一包数据没发完，*/
    USER_CTRL_SPP_SEND_DATA,
    USER_CTRL_SPP_UPDATA_DATA,
    //serial port profile disconnect command
    USER_CTRL_SPP_DISCONNECT,

    //蓝牙其他操作
//    //删除最新的一个设备记忆
//    USER_CTRL_DEL_LAST_REMOTE_INFO   ,
//    //删除所有设备记忆
//    USER_CTRL_DEL_ALL_REMOTE_INFO    ,
    USER_CTRL_TEST_KEY,

    USER_CTRL_CMD_RESERVE_INDEX1,
    USER_CTRL_CMD_RESERVE_INDEX2,
    USER_CTRL_CMD_RESERVE_INDEX3,
    USER_CTRL_CMD_RESERVE_INDEX4,
    USER_CTRL_CMD_RESERVE_INDEX5,
    USER_CTRL_LAST
} USER_CMD_TYPE;


////----反馈给客户使用的状态----////
typedef enum {
    /*下面是一些即时反馈的状态，无法重复获取的状态*/
    BT_STATUS_POWER_ON   = 1,   /*上电*/
    BT_STATUS_POWER_OFF  = 2,
    BT_STATUS_INIT_OK,          /*初始化完成*/
    BT_STATUS_FIRST_CONNECTED,        /*连接成功*/
    BT_STATUS_SECOND_CONNECTED,        /*连接成功*/
    BT_STATUS_FIRST_DISCONNECT,       /*断开连接*/
    BT_STATUS_SECOND_DISCONNECT,        /*断开连接*/
    BT_STATUS_PHONE_INCOME,     /*来电*/
    BT_STATUS_PHONE_NUMBER,     /*来电话好吗*/
    BT_STATUS_PHONE_OUT,        /*打出电话*/
    BT_STATUS_PHONE_ACTIVE,     /*接通电话*/
    BT_STATUS_PHONE_HANGUP,     /*挂断电话*/
    BT_STATUS_BEGIN_AUTO_CON,   /*发起回连*/
    BT_STATUS_MUSIC_SOUND_COME, /*库中加入auto mute判断音乐播放开始*/
    BT_STATUS_MUSIC_SOUND_GO,   /*库中加入auto mute判断音乐播放暂停*/
    BT_STATUS_RESUME,           /*后台有效，手动切回蓝牙*/
    BT_STATUS_RESUME_BTSTACK,   /*后台有效，后台时来电切回蓝牙*/
    BT_STATUS_SUSPEND,          /*蓝牙挂起，退出蓝牙*/
    BT_STATUS_LAST_CALL_TYPE_CHANGE,    /*最后拨打电话的类型，只区分打入和打出两种状态*/
    BT_STATUS_CALL_VOL_CHANGE,     /*通话过程中设置音量会产生这个状态变化*/
    BT_STATUS_SCO_STATUS_CHANGE,    /*当esco/sco连接或者断开时会产生这个状态变化*/
    BT_STATUS_CONNECT_WITHOUT_LINKKEY,   /*判断是首次连接还是配对后的连接，主要依据要不要简易配对或者pin code*/
    BT_STATUS_PHONE_BATTERY_CHANGE,     /*电话电量变化，该状态仅6个等级，0-5*/
    BT_STATUS_RECONNECT_LINKKEY_LOST,     /*回连时发现linkkey丢失了，即手机取消配对了*/
    BT_STATUS_RECONN_OR_CONN,       /*回连成功还是被连接*/
    BT_STATUS_BT_TEST_BOX_CMD,              /*蓝牙收到测试盒消息。1-升级，2-fast test*/
    BT_STATUS_SNIFF_DAC_CTL,              /*SNIFF操作dac*/
    BT_STATUS_TONE_BY_FILE_NAME, /*直接使用文件名播放提示音*/

    /*下面是1个持续的状态，是get_stereo_bt_connect_status获取*/
    BT_STATUS_STEREO_WAITING_CONN,/*page连接对箱中，还没连接上*/

    /*下面是6个持续的状态，是get_bt_connect_status()获取*/
    BT_STATUS_INITING,          /*正在初始化*/
    BT_STATUS_WAITINT_CONN,     /*等待连接*/
    BT_STATUS_AUTO_CONNECTINT,  /*正在回连*/
    BT_STATUS_CONNECTING,       /*已连接，没有电话和音乐在活动*/
    BT_STATUS_TAKEING_PHONE,    /*正在电话*/
    BT_STATUS_PLAYING_MUSIC,    /*正在音乐*/
} STATUS_FOR_USER;

typedef enum {
    BT_CALL_BATTERY_CHG = 0, //电池电量改变
    BT_CALL_SIGNAL_CHG,      //网络信号改变
    BT_CALL_INCOMING,   //电话打入
    BT_CALL_OUTGOING,   //电话打出
    BT_CALL_ACTIVE,     //接通电话
    BT_CALL_HANGUP,      //电话挂断
    BT_CALL_ALERT,       //远端reach
    BT_CALL_VOL_CHANGED,
} BT_CALL_IND_STA;

typedef enum {
    BT_MUSIC_STATUS_IDLE = 0,
    BT_MUSIC_STATUS_STARTING,
    BT_MUSIC_STATUS_SUSPENDING,
} BT_MUSIC_STATE;  //音乐状态

#define BT_PROMPT_EN     //任意时间按照文件号播文件

#define    SPP_CH       0x01
#define    HFP_CH       0x02
#define    A2DP_CH      0x04    //media
#define    AVCTP_CH     0x08
#define    HID_CH       0x10
#define    AVDTP_CH     0x20

enum {
    UPDATA_OSC_TRIM = 1,
    UPDATA_OTHER,
};

typedef void (*bt_simple_fun_handle)(void);

extern u32 user_send_cmd_prepare(USER_CMD_TYPE cmd, u16 param_len, u8 *param);

/*
u8 get_curr_channel_state();  与  channel  判断区分
主动获取当前链路的连接状态，可以用来判断有哪些链路连接上了
*/
extern u8 get_curr_channel_state();
/*
u8 get_call_status(); 与BT_CALL_IND_STA 枚举的值判断
用于获取当前蓝牙电话的状态
*/
extern u8 get_call_status();
extern void user_cmd_ctrl_init(void *var);

extern void bt_info_init(void);
extern void bt_test_info_init(void);
extern void hook_bt_trim_backup(s16 i_dc, s16 q_dc);
extern u8 hook_bt_trim_check_crc(void);

/*个性化参数设置*/
extern void bt_cfg_default_init(u8 support);
/*用户调试设置地址，6个byte*/
extern void __set_bt_mac_addr(u8 *addr);

extern void __set_use_default_value(u8 mode);
/*用户调试设置name,最长32个字符*/
extern void __set_host_name(const char *name, u8 len);
/*用户调试设置pin code*/
extern void __set_pin_code(const char *code);
/*该接口用于设置上电回连需要依次搜索设备的个数。*/
extern void __set_auto_conn_device_num(u8 num);

/*设置对箱搜索标识，inquiry时候用,搜索到相应的标识才允许连接*/
void __set_stereo_device_indicate(const char *indicate);

/*设置设备做主还是做从*/
void __set_device_role(u8 role) ;

/*//回连的超时设置。ms单位。但是对手机发起的连接是没作用的*/
extern void __set_super_timeout_value(u32 time);
/*接口配置是测试盒模式还是蓝牙SDK*/
extern void __set_test_masetr_enable(u8 en);
/*外部设置支持什么协议*/
extern void bt_cfg_default_init(u8 support);

/*提供动态设置音乐自动播放的接口。
  该接口使用的时间点有要求，要在A2DP连接之前设置
   */
extern void __bt_set_a2dp_auto_play_flag(u8 flag);


/*提供接口外部设置配对方式*/
extern void __set_simple_pair_flag(u8 flag);

extern bool get_remote_test_flag();
/*//回连page的超时设置。ms单位*/
extern void __set_page_timeout_value(u16 time);
extern void __set_soundtrack_compound(u8 enable);
/*//设置开启蓝牙可发现可连接的时间，为了省电，设置一个自动关闭可连接的时间。ms单位。
    为 0 时不使用自动关闭*/
extern void __set_connect_scan_timeout_value(u32 time);
/*设置电量显示发送更新的周期时间，为0表示关闭电量显示功能*/
void __bt_set_update_battery_time(u32 time);
/*给用户设置蓝牙支持连接的个数，主要用于控制控制可发现可连接和回连流程*/
extern void __set_user_ctrl_conn_num(u8 num);
/*是否支持电量显示功能*/
extern void __bt_set_display_battery_enable(u8 flag);
/*提供接口外部设置要保留hfp不要蓝牙通话*/
extern void __set_disable_sco_flag(u8 flag);
/*高级音频抢断开关*/
extern void __set_music_break_in_flag(u8 flag);
extern void __set_auto_pause_flag(u8 flag);
/*通话抢断开关，0：不抢断，1：抢断*/
extern void __set_hfp_switch(u8 switch_en);
/*调整内部电容0-16*/
extern void __set_xosc_value(u8 value);
extern void __set_esco_packet_type(u8 type);  /*esco packet  */
/*设置蓝牙搜索的时间。ms单位。蓝牙inquiry搜索用*/
extern void __set_searche_timeout_value(u16 time);
extern void __set_sbc_cap_bitpool(u16 sbc_cap_bitpoola);

/*有些自选接口用来实现个性化功能流程，回调函数注册，记得常来看看哟*/
extern void get_battery_value_register(int (*handle)(void));    /*电量发送时获取电量等级的接口注册*/
extern void music_vol_change_handle_register(void (*handle)(int vol), int (*handle2)(void)); /*手机更样机音乐模式的音量同步*/
extern void read_remote_name_handle_register(void (*handle)(u8 *name));   /*获取到名字后的回调函数接口注册函数*/
extern void spp_data_deal_handle_register(void (*handler)(u8 packet_type, u16 channel, u8 *packet, u16 size)); /*支持串口功能的数据处理接口*/
extern void discon_complete_handle_register(void (*handle)(u8 *addr, int reason)); /*断开或者连接上会调用的函数，给客户反馈信息*/

extern void bt_fast_test_handle_register(void (*handle)(void));
extern void bt_updata_handle_register(void (*start_handle)(u32), void (*end_handle)(u32));
extern s32 bt_updata_test_user(void);
extern u32 get_updata_end_flag(void);

/*下面客户一般用不到的*/
extern u8 get_bt_newest_status(void);
extern void update_bt_current_status(u8 *addr, u8 new_status, u8 conn_status);
extern void hook_user_deal_hfp_status(u8 *addr, u32 sta, int param);
extern u8 get_bt_connect_status(void);


extern void updata_profile_channels_status(u8 *addr, int state, int channel);
extern void hook_bt_trim_backup_register(void (*handle)(s16 i_dc, s16 q_dc));
extern void bt_discon_complete_handle(u8 *addr, int reason);

extern void cfg_bt_power_osc_init(u8 osc, u32 osc_hz, u8 rtc_2pin_32k, u8 set_PR1);
extern void cfg_test_box_and_work_mode(u8 test_box, u8 work_mode);
extern void cfg_bt_pll_para(u32 osc, u32 sys, u8 low_power, u8 xosc);
extern void cfg_bredr_mem(u8 *rx, u32 rx_len, u8 *tx, u32 tx_len);
extern void __bt_set_sniff(u8 sniff_mode_config);
extern u8 check_is_poweroff_up();
extern tbool is_1t2_connection(void);
extern u8 get_total_connect_dev(void);
extern u8 get_current_search_index();
extern u8 app_get_is_poweroff_up();

extern void set_bt_info_callback(void (*fun)(void));
extern void user_prote_bt_process(u8 mode);
extern void __set_connect_scan_with_key(u8 flag);
extern void __set_ble_bredr_mode(u8 flag);
extern void register_edr_init_handle(void);
extern void register_ble_init_handle(void);
extern void fcc_test_handle_register(void (*handle)(void), void (*handle2)());
extern bool get_suspend_flag(void);
extern u8 is_bt_conn_hfp_hangup(u8 *addr);

extern void infor_2_user_handle_register(int (*handle)(u8 *info, u16 len), u8 *buffer_ptr);



extern bool get_resume_flag();



extern void ble_RF_test_uart_para_init(char *uart_name, u32 baud, u8 io);
extern void cfg_ble_work_mode(u8 work_mode);

extern void __set_authentication_cover(u8 cover);


#define BD_CLASS_WEARABLE_HEADSET	0x240404/*ios10.2 display headset icon*/
#define BD_CLASS_HANDS_FREE			0x240408/*ios10.2 display bluetooth icon*/
#define BD_CLASS_MICROPHONE			0x240410
#define BD_CLASS_LOUDSPEAKER		0x240414
#define BD_CLASS_HEADPHONES			0x240418
#define BD_CLASS_CAR_AUDIO			0x240420
#define BD_CLASS_HIFI_AUDIO			0x240428
extern void __change_hci_class_type(u32 class);
void bt_phone_audio_handle_register(s32(*sco_conn_cb)(void *priv),
                                    s32(*sco_disconn_cb)(void *priv),
                                    void (*sco_rx_cb)(u8 *data, u16 point, u8 sco_flags));

extern void bt_a2dp_audio_handle_register(void *(*start_handle)(void *priv, const char *format,
        u16(*read_handle)(void *priv, u8 *buf, u16 len),
        bool (*seek_handle)(void *priv, u8 type, u32 offsiz)),
        void (*stop_handle)(void **hdl));

void bt_mask_callback_init();
void bt_config_default_init();
u8 bt_stack_all_mem_init(u8 *buf, u32 len, u8 *overlay, u32 overlay_len);
extern int maskrom_btstack_cmd_deal(u16 cmd, u8 mac_addr[6]);
bool is_sniff_mode();
bool user_sniff_check_req(u8 sniff_cnt_time);
bool user_sniff_check_req(u8 sniff_cnt_time);
void resource_manage_init_app();
bool user_sniff_check_req(u8 sniff_cnt_time);
bool get_hfp_active_addr(u8 *addr, u8 *active_addr);
s32 bt_noconn_pwr_handle_register(u32(*in_fun)(void), u32(*out_fun)(void));
bool check_is_connect_sniff_status(void);
void clear_sniff_cnt(u8 mode);
u8 check_support_inband_ringtone(void);
u8 get_second_call_status();
u8 get_cur_conn_support_inband_ringtone();
u8 user_get_total_connect_dev();
void __set_w_sco_slot(u8 two_con_w_sco_slot, u8 one_w_sco_slot);
#endif

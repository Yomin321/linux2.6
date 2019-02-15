/*********************************************************************************************
    *   Filename        : sdk_cfg.h

    *   Description     : Config for Sound Box Case

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-12-01 15:36

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef _AC691X_SDK_CONFIG_
#define _AC691X_SDK_CONFIG_

#include "includes.h"

// *INDENT-OFF*
/********************************************************************************/

/********************************************************************************/
/********************************************************************************/
/*
 *           --------芯片选型配置
 */

///可选配置AC6910_LQFP80/AC6916D_QFN32/AC6917A_TSSOP24/AC6918A_TSSOP20/AC6918B_TSSOP20/AC6919A_SOP16/AC6919B_SOP16/AC6919C_SOP16/AC6919D_SOP16
#define  AC691X_CPU_CONFIG    AC6910_LQFP80 //       AC6918A_TSSOP20

#if (AC691X_CPU_CONFIG==AC6919A_SOP16)
   #include "ac691x_cfg/ac6919a_cfg.h"
#elif (AC691X_CPU_CONFIG==AC6919C_SOP16)
   #include "ac691x_cfg/ac6919c_cfg.h"
#elif (AC691X_CPU_CONFIG==AC6910_LQFP80)
   #include "ac691x_cfg/ac6910_cfg.h"
#elif (AC691X_CPU_CONFIG==AC6916D_QFN32)
   #include "ac691x_cfg/ac6916d_cfg.h"
#elif (AC691X_CPU_CONFIG==AC6918A_TSSOP20)
   #include "ac691x_cfg/ac6918a_cfg.h"
#elif (AC691X_CPU_CONFIG==AC6917A_TSSOP24)
   #include "ac691x_cfg/ac6917a_cfg.h"
#else
  #error  "-------please check AC691X_CPU_CONFIG------------"
#endif


/********************************************************************************/
/********************************************************************************/
/*
 *           --------电量监测配置
 */
#define SYS_LVD_EN                   1

///<自动mute
#define DAC_AUTO_MUTE_EN		     1

///<按键音
#define TONE_EN     	    	     1

#define BT_PHONE_NUMBER              1
//需要电量显示但是不需要通话功能
#define BT_HFP_EN_SCO_DIS              0


///<非0表示使用默认音量
#define SYS_DEFAULT_VOL              23
#define TONE_DEFAULT_VOL             0

#define CHECK_PULL_UP_EN             0////根据上拉电阻选择设备名字使能

///<通话的时候使用差分输出(前提是VCOMO_EN为1)
#define CALL_USE_DIFF_OUTPUT		0

#if (KEY_AD_RTCVDD_EN==0)
   #undef  CHECK_PULL_UP_EN
   #define CHECK_PULL_UP_EN         0
#endif

///可选配置：NORMAL_MODE/TEST_BQB_MODE/TEST_FCC_MODE/TEST_FRE_OFF_MODE/TEST_BOX_MODE/TEST_PERFOR_MODE
#define BT_MODE                      NORMAL_MODE		//TEST_BQB_MODE//

//开机未连接时功率大小设置0~9
#define BT_DISCONNECT_MAX_PWR              4


/// if test mode close sniff and low power
#if (BT_MODE != NORMAL_MODE)

	#undef  SNIFF_MODE_CONF
	#define SNIFF_MODE_CONF           0

	#undef  BT_LOW_POWER_MODE
	#define BT_LOW_POWER_MODE         0

	#undef  BT_CONNTCT_NUM
	#define BT_CONNTCT_NUM            1

#endif

//支持播放设备

#define  SURPORT_DEVICE

///<蓝牙晶振频偏设置
#define BT_OSC_INTERNAL_L           0x12//蓝牙晶振频偏L设置 0x0~0x1f
#define BT_OSC_INTERNAL_R           0x12//蓝牙晶振频偏R设置 0x0~0x1f

///可选配置：BT_OSC/RTC_OSCH/RTC_OSCL/LRC_32K
#define LOWPOWER_OSC_TYPE           LRC_32K

///可选配置：32768L//24000000L//32000L
#define LOWPOWER_OSC_HZ             32000L

///可选配置：BT_BREDR_EN/BT_BLE_EN/(BT_BREDR_EN|BT_BLE_EN)
#define BLE_BREDR_MODE              (BT_BREDR_EN)


///---sdp service record profile- 用户选择支持协议--///
#define USER_SUPPORT_PROFILE_SPP    1
#define USER_SUPPORT_PROFILE_HFP    1
#define USER_SUPPORT_PROFILE_A2DP   1
#define USER_SUPPORT_PROFILE_AVCTP  1
#define USER_SUPPORT_PROFILE_HID    1



#endif
// *INDENT-ON*

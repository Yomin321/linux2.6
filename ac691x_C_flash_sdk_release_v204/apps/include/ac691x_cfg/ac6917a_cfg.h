
#ifndef _AC6917A_CONFIG_
#define _AC6917A_CONFIG_

#include "includes.h"

/*按键类型定义*/
#define KEY_AD_RTCVDD_EN       0   ///<采用RTCVDD电源的AD按键使能
#define KEY_AD_VDDIO_EN        0   ///<采用VDDIO电源的AD按键使能
#define KEY_IO_EN              1   ///<IO按键使能
#define KEY_IR_EN              0   ///<红外遥控使能,PA9
#define KEY_TCH_EN             0   ///<触摸按键使能
#define KEY_UART_EN            0   ///<串口按键使能

///可选配置KEY_PORT_PR/KEY_PORT_DP/KEY_PORT_DM/KEY_POARA/KEY_POARA/KEY_POARB/KEY_POARC/KEY_POARD/ 0
#define IO_KEY_PP_POART_SLECT       KEY_PORT_PR
///可选配置KEY_PORTR_0~KEY_PORTR_3/ PORT_BIT_0~PORT_BIT_15/ 0
#define IO_KEY_PP_N                 KEY_PORTR_2     ///use witch port 做PP按键



///可选配置KEY_PORT_PR/KEY_PORT_DP/KEY_PORT_DM/KEY_POARA/KEY_POARA/KEY_POARB/KEY_POARC/KEY_POARD/ 0
#define IO_KEY_NEXT_POART_SLECT     KEY_PORT_DM
//可选配置KEY_PORTR_0~KEY_PORTR_3/ PORT_BIT_0~PORT_BIT_15/ 0
#define IO_KEY_NEXT_N               0        ///use witch port 做NEXT按键



///可选配置KEY_PORT_PR/KEY_PORT_DP/KEY_PORT_DM/KEY_POARA/KEY_POARA/KEY_POARB/KEY_POARC/KEY_POARD/ 0
#define IO_KEY_PREV_POART_SLECT     KEY_PORT_DP
///可选配置KEY_PORTR_0~KEY_PORTR_3/ PORT_BIT_0~PORT_BIT_15/ 0
#define IO_KEY_PREV_N               0        ///use witch port 做PREV按键

// *INDENT-OFF*
/********************************************************************************/

/********************************************************************************/
/*
 *           --------LED配置
 */
///<LED指示使能
#define LED_EN                       1

//用usb 口控制灯选择 可选配置LED_DM_ONE_LED/LED_DM_TWO_LED/LED_DM_DP/ 0
#define USE_USB_IO_LED               0

#define USE_PWM_CONTROL_LED          1       //用pwm推灯设置,用usb 的DM同时推两个灯R_LED, B_LED,只能用普通IO口推

#define LED_B_PORT_SLECT         LED_POARA
#define LED_B                    PORT_BIT_4 ///点蓝灯用第几个口

#define LED_R_PORT_SLECT         LED_POARA
#define LED_R                    PORT_BIT_3 ///点红灯用第几个口

//可选配置 LED_POARA/LED_POARB/LED_POARC/LED_POARD
#define LEDB_PORT                 (LED_B_PORT_SLECT|LED_B)//设置用哪个POART口，第几个口
#define LEDR_PORT                 (LED_R_PORT_SLECT|LED_R)//设置用哪个POART口，第几个口

#if USE_USB_IO_LED
   #undef  LEDB_PORT
   #undef  LEDR_PORT
   #define LEDB_PORT                  0
   #define LEDR_PORT                  0
#endif





///<用内部充电
#define CHARGE_PROTECT_EN            1


///可选配置：PWR_LDO33/PWR_LDO15/PWR_DCDC15
#define PWR_MODE_SELECT              PWR_DCDC15


///蓝牙无连接自动关机计时，u16类型，0表示不自动关机
#define AUTO_SHUT_DOWN_TIME          190 //除以2 减去进低功耗的时间 概数



///<按键双击功能 ,可选配置KEY_DOUBLE_CLICK_PP/KEY_DOUBLE_CLICK_VOL_DOWN/KEY_DOUBLE_CLICK_VOL_UP/0

#define KEY_DOUBLE_CLICK             (KEY_DOUBLE_CLICK_PP)//KEY_DOUBLE_CLICK_PP|KEY_DOUBLE_CLICK_VOL_DOWN

///<vddio rtcvdd level
#define SYS_VDDIO_LEVEL	             1 	//range:0~7
#define SYS_RTCVDD_LEVEL	         1	//range:0~7


/********************************************************************************/

/********************************************************************************/
/*
 *           --------音效类配置
 */
///<dac声道合并
#define DAC_SOUNDTRACK_COMPOUND      0

///<dac差分输出
#define DAC_DIFF_OUTPUT		 	     0

#if DAC_DIFF_OUTPUT
  #define DAC_DIFF_OUTPUT_NO_VCM		 0
#else
  #define DAC_DIFF_OUTPUT_NO_VCM		 0
#endif

///功放可选配置：PA_MUTE_HIGH/PA_MUTE_LOW/0
#define PA_MUTE_CFG                  0

/********************************************************************************/


/********************************************************************************/
/*
 *           --------DAC VCOMO 配置
 */
///是否选择VCMO直推耳机
#define VCOMO_EN 	                 1

///可选配置：DAC_L_R_CHANNEL/DAC_L_CHANNEL/DAC_R_CHANNEL
#define DAC_CHANNEL_SLECT            DAC_L_R_CHANNEL



/********************************************************************************/

/********************************************************************************/
/*
 *           --------蓝牙类配置
 */
#include "bluetooth/bluetooth_api.h"


/*
 *           --------蓝牙低功耗设置
 *   使能该功能后只能是纯蓝牙功能，没有显示功能
 */
///可选配置：SNIFF_EN/SNIFF_TOW_CONN_ENTER_POWERDOWN_EN
#define SNIFF_MODE_CONF            SNIFF_EN

///可选配置：BT_POWER_DOWN_EN/BT_POWER_OFF_EN
#define BT_LOW_POWER_MODE          BT_POWER_DOWN_EN


///蓝牙连接个数选择 1 /2 一拖二
#define BT_CONNTCT_NUM             2



#endif//end_sdk_cfg
// *INDENT-OFF*

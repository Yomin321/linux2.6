#ifndef _INCLUDES_
#define _INCLUDES_

#include "typedef.h"
#include "common/printf.h"
#include "string.h"

/********************************************************************************/

/********************************************************************************/
/*
 *           --------芯片选型配置
 */
#define AC6910_LQFP80        1

#define AC6916D_QFN32        2   //高端立体声蓝牙耳机，带DCDC，功耗低，封装体积小
#define AC6916H_QFN32        3   //

#define AC6917A_TSSOP24      4   //高端立体声蓝牙耳机，带DCDC，功耗低

#define AC6918A_TSSOP20      5   //高端单声道蓝牙耳机，带DCDC，功耗低
#define AC6918B_TSSOP20      6   //立体声蓝牙耳机


#define AC6919A_SOP16        7   //单声道蓝牙耳机
#define AC6919B_SOP16        8   //
#define AC6919C_SOP16        9   //单声道纯蓝牙音箱
#define AC6919D_SOP16        10  //

/********************************************************************************/

/********************************************************************************/
/*
 *           --------电源类配置
 */
///   1:  LDOIN 5v -> LDO   1.5v -> DVDD 1.2v, support bluetooth
#define    PWR_LDO15            1
///   2:  LDOIN 5v -> DCDC  1.5v -> DVDD 1.2v, support bluetooth
#define    PWR_DCDC15           2


/********************************************************************************/

/********************************************************************************/
/*
 *           --------外设类配置
 */
#define SDMMC0_EN           1
#define SDMMC1_EN           1
#define USB_DISK_EN         0
#define USB_PC_EN           1



//上电检测功能 1：上电检测切模式   0：上电不检测默认进蓝牙
#define POWERUP_DET_EN          0

///<模拟配置
#define BT_ANALOG_CFG           0
#define BT_XOSC                 0

#define BT_OSC              0
#define RTC_OSCH            1
#define RTC_OSCL            2
#define LRC_32K             3
// #define MAIN_TASK_NAME        "MainTask"
// #define RTC_TASK_NAME         "RTCTask"
// #define MUSIC_TASK_NAME       "MusicTask"
// #define RECORD_TASK_NAME      "RECTask"
// #define FM_TASK_NAME		  "FMRadioTask"
// #define LINEIN_TASK_NAME      "LineinTask"
// #define UDISK_TASK_NAME       "USBdevTask"
// #define BTMSG_TASK_NAME       "btmsg"
// #define BTSTACK_TASK_NAME     "BtStackTask"
// #define IDLE_TASK_NAME        "IdleTask"
// #define RCSP_TASK_NAME 		  "RCSP_Task"
// #define RDEF_LIGHT_TASK_NAME  "LightTask"

/********************************************************************************/


/********************************************************************************/
/*
 *           --------MUSIC MACRO
 */
///<MP3
#define DEC_TYPE_MP3_ENABLE     1
///<SBC
#define DEC_TYPE_SBC_ENABLE     1
///<AAC
#define DEC_TYPE_AAC_ENABLE	    0

///<3K_code_space
#define DEC_TYPE_WAV_ENABLE     1
///<5K_code_space
#define DEC_TYPE_FLAC_ENABLE    0
///<8K_code_space
#define DEC_TYPE_APE_ENABLE     0
///<30K_code_space
#define DEC_TYPE_WMA_ENABLE     0
///<30K_code_space
#define DEC_TYPE_F1A_ENABLE     0
///<2K_code_space
#define DEC_TYPE_NWT_ENABLE     0



///<EQ模块开关
#define EQ_EN			        1
#define AUDIO_EFFECT_HW_EQ      1
//EQ 串口在线调试使能
#define UART1_TXPB0_RXPB1       0x01
#define UART1_USB_TXDP_RXDM     0X02
#if EQ_EN
#define EQ_UART_DEBUG           0
#else
#define EQ_UART_DEBUG           0
#endif
//在线调EQ 串口选择，可选：UART1_TXPB0_RXPB1 / UART1_USB_TXDP_RXDM
#define EQ_DEBUG_UART_SEL       UART1_USB_TXDP_RXDM     // EQ_UART_DEBUG 为 1 有效
#define EQ_DEBUG_UART_RATE      9600


/********************************************************************************/
#define REC_EN			    1
#define REC_TASK_EN		    1



/********************************************************************************/

/********************************************************************************/

//PR PORT
#define  KEY_PORTR_0       0
#define  KEY_PORTR_1       1
#define  KEY_PORTR_2       2
#define  KEY_PORTR_3       3

#define  PORT_BIT_0      BIT(0)
#define  PORT_BIT_1      BIT(1)
#define  PORT_BIT_2      BIT(2)
#define  PORT_BIT_3      BIT(3)
#define  PORT_BIT_4      BIT(4)
#define  PORT_BIT_5      BIT(5)
#define  PORT_BIT_6      BIT(6)
#define  PORT_BIT_7      BIT(7)
#define  PORT_BIT_8      BIT(8)
#define  PORT_BIT_9      BIT(9)
#define  PORT_BIT_10     BIT(10)
#define  PORT_BIT_11     BIT(11)
#define  PORT_BIT_12     BIT(12)
#define  PORT_BIT_13     BIT(13)
#define  PORT_BIT_14     BIT(14)
#define  PORT_BIT_15     BIT(15)

#define LED_DM_ONE_LED  BIT(25) //用usb 的DM B_LED
#define LED_DM_TWO_LED  BIT(26) //用usb 的DM同时推两个灯R_LED, B_LED,推灯注意开关灯的顺序
#define LED_DM_DP       BIT(27) //用usb dm dp推灯时用此宏,用usb推灯
#define LED_POARA       BIT(28)
#define LED_POARB       BIT(29)
#define LED_POARC       BIT(30)
#define LED_POARD       BIT(31)




#define KEY_PORT_PR     BIT(25)
#define KEY_PORT_DP     BIT(26)
#define KEY_PORT_DM     BIT(27)
#define KEY_POARA       BIT(28)
#define KEY_POARB       BIT(29)
#define KEY_POARC       BIT(30)
#define KEY_POARD       BIT(31)

#define  PA_MUTE_HIGH   BIT(0) //高mute
#define  PA_MUTE_LOW    BIT(1) //低mute


#define KEY_DOUBLE_CLICK_PP         BIT(0)//双击PP键生效
#define KEY_DOUBLE_CLICK_VOL_DOWN   BIT(1)//双击减键生效
#define KEY_DOUBLE_CLICK_VOL_UP     BIT(2)//双击加键生效


enum {
    MODE_CHANCE_10K = 0,  //10k   I7
    MODE_CHANCE_22K,      //22k   S6
    MODE_CHANC_33K,       //33k   001  M165 //Bluetooth music
    MODE_CHANCE_51K,      //51k
    MODE_CHANCE_NULL,     //null  S530
    MAX_MODE_CHANCE,
};

/********************************************************************************/
#define LINEIN_EN		    0			//linein_task
#define AUX_DETECT_EN	    1			//linein_detect
#define REC_PLAY_EN			0		//not support


#endif


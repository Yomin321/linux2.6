#ifndef	_USB_SLAVE_CTL_H_
#define _USB_SLAVE_CTL_H_

#include "typedef.h"

//----------------------------------
// Descriptor argument
//----------------------------------
#define USB_SLAVE_INF		        (MASSSTORAGE_INF)
#define USB_SLAVE_CFG		        (MASSSTORAGE_CFG + 9)

///////////MassStorage Class
#define	    BULK_EP_OUT		     		1
#define	    BULK_EP_IN		     		1

#define	    MAXP_SIZE_BULKOUT			64
#define	    MAXP_SIZE_BULKIN			64

#define 	MASSSTORAGE_CFG             (23)
#define 	MASSSTORAGE_INF              1


/////////////Audio Class
//#define     ISO_EP_IN           	 	2
#define     ISO_EP_IN           	 	3
#define     ISO_EP_OUT           		2

#define     SPK_AUDIO_RATE              48L
#define		SPK_AUDIO_RES		        16

#define		SPK_CHANNEL			        2
#define		SPK_FRAME_LEN		        (SPK_AUDIO_RATE * SPK_AUDIO_RES / 8 * SPK_CHANNEL)

#define		SPK_PCM_Type		        (SPK_AUDIO_RES >> 4)				// 0=8 ,1=16
#define		SPK_AUDIO_TYPE		        (0x02 - SPK_PCM_Type)			// TYPE1_PCM16


#define     MIC_AUDIO_RATE              48
#define		MIC_AUDIO_RES				16

#define		MIC_CHANNEL			        2
#define		MIC_FRAME_LEN		        (MIC_AUDIO_RATE * MIC_AUDIO_RES / 8 * MIC_CHANNEL)

#define		MIC_PCM_Type		        (MIC_AUDIO_RES >> 4)				// 0=8 ,1=16
#define		MIC_AUDIO_TYPE		        (0x02 - MIC_PCM_Type)			// TYPE1_PCM16


#define 	AUDIO_CFG                   (0x66 + 0x52)
#define 	AUDIO_INF                   3


#define     SPEAKER_CFG       	        (0x65) 	//AC-S(0x9) + AC-C(0x9) + IT(0xC) + FU(0xA) + OT(0x9) + AS-S0(0x9) + AS-S1(0x9) + AS-C(0x7) + AS-C(0xB) +
#define     SPEAKER_INF       	        (2)
#define     SPEAKER_AC			        (0x28)

#define     MIC_CFG       		        (0x64)
#define     MIC_INF       		        (2)
#define     MIC_AC				        (0x27)



////////////HID Class
//#define     HID_EP_IN            		3
#define     HID_EP_IN            		2
//#define     HID_EP_OUT           		2
#define     HID_CFG                     (25)
#define     HID_INF                     1
#define     HidReportDescriptors_len    51
#define     HID_MAX_SIZE                8




#define DEV_LEN     0x12      // Device Descriptor Length

#define CFG_LEN     (USB_AUDIO_CFG + CARD_READER_CFG + USB_HID_CFG + 9)     // Configuration Descriptor Length

#define INF_LEN     (USB_AUDIO_INF + CARD_READER_INF + USB_HID_INF)


/*Main Items*/
#define INPUT(x)              (0x80 | 0x1), x
#define OUTPUT(x)             (0x90 | 0x1), x
#define COLLECTION(x)         (0xA0 | 0x1), x
#define FEATURE(x)            (0xB0 | 0x1), x
#define END_COLLECTION        0xC0

/*Golbal Items*/
#define USAGE_PAGE(x)         (0x04 | 0x1), x
#define LOGICAL_MIN(x)        (0x14 | 0x1), x
#define LOGICAL_MAX(x)        (0x24 | 0x1), x
#define PHYSICAL_MIN(x)       (0x34 | 0x1), x
#define PHYSICAL_MAX(x)       (0x44 | 0x1), x
#define UNIT_EXPONENT(x)      (0x54 | 0x1), x
#define UNIT(x)               (0x64 | 0x1), x
#define REPORT_SIZE(x)        (0x74 | 0x1), x
#define REPORT_ID(x)          (0x84 | 0x1), x
#define REPORT_COUNT(x)       (0x94 | 0x1), x
#define PUSH(x)               (0xA4 | 0x1), x
#define POP(x)                (0xB4 | 0x1), x

/*Local Items*/
#define USAGE(x)              (0x08 | 0x1), x
#define USAGE2(x)             (0x08 | 0x2), x
#define USAGE_MIN(x)          (0x18 | 0x1), x
#define USAGE_MAX(x)          (0x28 | 0x1), x
#define DESIGNATOR_INDEX(x)   (0x38 | 0x1), x
#define DESIGNATOR_MIN(x)     (0x48 | 0x1), x
#define DESIGNATOR_MAX(x)     (0x58 | 0x1), x
#define STRING_INDEX(x)       (0x78 | 0x1), x
#define STRING_MIN(x)         (0x88 | 0x1), x
#define STRING_MAX(x)         (0x98 | 0x1), x
#define DELIMITER(x)          (0xA8 | 0x1), x


/*Consumer Page*/
#define CONSUMER_PAGE           0x0C
#define CONSUMER_CONTROL        0x01

//Collection
#define PHYSICAL                0x00
#define APPLICATION             0x01
#define LOGICAL                 0x02
#define REPORT                  0x03

#define PLAY                    0xB0
#define PAUSE                   0xB1
#define RECORD                  0xB2
#define FAST_FORWARD            0xB3
#define REWIND                  0xB4
#define SCAN_NEXT_TRACK         0xB5
#define SCAN_PREV_TRACK         0xB6
#define STOP                    0xB7

#define FRAME_FORWARD           0xC0
#define FRAME_BACK              0xC1
#define TRACKING_INC            0xCA
#define TRACKING_DEC            0xCB
#define STOP_EJECT              0xCC
#define PLAY_PAUSE              0xCD
#define PLAY_SKIP               0xCE

#define VOLUME                  0xE0
#define BALANCE                 0xE1
#define MUTE                    0xE2
#define BASS                    0xE3
#define VOLUME_INC              0xE9
#define VOLUME_DEC              0xEA

#define BALANCE_LEFT            0x50, 0x01
#define BALANCE_RIGHT           0x51, 0x01
#define CHANNEL_LEFT            0x61, 0x01
#define CHANNEL_RIGHT           0x62, 0x01
//----------------------------------
// HID key for audio
//----------------------------------
#define USB_AUDIO_STOP          0x8000
#define USB_AUDIO_FASTFORWARD   0x4000
#define USB_AUDIO_PREFILE       0x2000
#define USB_AUDIO_NEXTFILE      0x1000
#define USB_AUDIO_PP            0x0800
#define USB_AUDIO_MUTE          0x0400
#define USB_AUDIO_VOLDOWN       0x0200
#define USB_AUDIO_VOLUP         0x0100
#define USB_AUDIO_NONE          0x0000

#define USB_AUDIO_PAUSE         0x0080
#define USB_AUDIO_PLAY          0x0040
#define USB_AUDIO_BALANCE_LEFT  0x0020
#define USB_AUDIO_BALANCE_RIGHT 0x0010
#define USB_AUDIO_VOLUME        0x0008
#define USB_AUDIO_STOP_EJECT    0x0004
#define USB_AUDIO_TRACKING_DEC  0x0002
#define USB_AUDIO_TRACKING_INC  0x0001
#define USB_AUDIO_NONE          0x0000


/**************************************
		EP0
**************************************/
#define EP0_SETUP_LEN		0x40
#define CODE		        0x0
#define XDATA		        0x1

///USB Slave 控制传输各阶段
#define Ep0_SETUP       0
#define Ep0_DATA_TX     1
#define Ep0_DATA_RX     2


typedef struct _USBS_REQUEST_BYTE {	//<USB 事务
    u8 bmRequestType;	//<[7]:传输方向 [6:5]:类型 [4:0]:接收方
    u8 bRequset;
    u8 wValueL;
    u8 wValueH;
    u8 wIndexL;
    u8 wIndexH;
    u8 wLengthL;
    u8 wLengthH;
} sUSBS_REQUEST_BYTE;


typedef union __USB_REQUEST_BYTE {
    sUSBS_REQUEST_BYTE SETUP_PKT;
    u8 USB_EP0_FIFO[0x40];
} uUSB_REQUEST_BYTE;


/*----------------------------------------------------------------------------*/
/**@brief   USB Slave 挂载，控制器初始化，I/O设置为USB模式
   @param   无
   @return  0：挂载成功； 1：挂载失败，详见错误列表
   @note    u32 usb_device_mount(void)
*/
/*----------------------------------------------------------------------------*/
u32 usb_device_mount(void);
/*----------------------------------------------------------------------------*/
/**@brief   USB 从机卸载，I/O恢复普通I/O模式，关闭USB中断
   @param   void
   @return
   @note	void usb_device_unmount(void)
*/
/*----------------------------------------------------------------------------*/
u32 usb_device_unmount(void);


void Ep0_control_transfer(void);
void Ep0_Data_Tx(void);
void Ep0_Data_Rx(void);
void usb_hid_key(u32 key);
void set_descriptor(void);
void susb_msg_set_hook(void *func);

#endif	/*	_USB_SLAVE_CTL_H_	*/

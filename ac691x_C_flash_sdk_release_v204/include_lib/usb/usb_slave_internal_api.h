#ifndef	_USB_SLAVE_INTERNAL_API_H_
#define _USB_SLAVE_INTERNAL_API_H_

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

//对应msg.h
enum {
    USB_SLAVE_CMD_MUTE		= 0x502,
    USB_SLAVE_CMD_UNMUTE	= 0x503,
    USB_SLAVE_CMD_VOL		= 0x504,
    USB_SLAVE_CMD_UPDATA	= 0x50b,
};

//----------------------------------
// IO CONTROL CMD
//----------------------------------
enum {
    USB_SLAVE_CLASS_CONFIG_RESET = 1,
    USB_SLAVE_CARD_READER0_REG,
    USB_SLAVE_CARD_READER1_REG,
    USB_SLAVE_GET_SPEAKER_VOL,
    USB_SLAVE_ONLINE_DETECT,
    USB_SLAVE_GET_ONLINE_STATUS,
    USB_SLAVE_RUN_CARD_READER,
    USB_SLAVE_HID,
    USB_SLAVE_MD_PROTECT,
    USB_SLAVE_SPK_OUTPUT_IO_REG,
};

typedef struct _usb_slave_dev_io {
    s32(*dev_init)(void *parm);
    s32(*dev_open)(void *parm);
    s32(*dev_close)(void *parm);
    s32(*dev_ioctrl)(void *parm, u32 cmd);
} usb_slave_dev_io;


extern const usb_slave_dev_io susb_io;

// extern function
void usb_class_setting(u8 config);
u32 get_pc_volume(void);
void usb_hid_control(u32 key);
void reg_usb_pc_protect(void *protect_param);
void usb_device_default_register(void);
void recover_cardreader_popup_api(void);
//void register_spk_cb_func(u32(*spk_cb)(s16 *, u32));
void register_spk_output_io(AUDIO_OUTPUT_IO *output_io);
void usb_mic_ladc_input(s16 *buf, u32 inlen);

void reg_usb_scsi_reg(void *fun);
#endif	/*	_USB_SLAVE_API_H_	*/

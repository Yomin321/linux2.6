#ifndef	_USB_SLAVE_SCSI_H_
#define _USB_SLAVE_SCSI_H_

// #include "USB_SCSI.h"
#include "usb_slave_api.h"

extern const u8 SCSIInquiryData[];
extern const u8 SCSIInquiryData1[];

/*----------------------------------------------------------------------------*/
/**@brief   USB Speaker ISO 传输处理函数
   @param   void
   @return  void
   @note    void usb_speaker_transfer(void)
*/
/*----------------------------------------------------------------------------*/
void usb_speaker_transfer(void);
/*----------------------------------------------------------------------------*/
/**@brief   USB Mic ISO 传输处理函数
   @param   void
   @return  无
   @note    void usb_mic_transfer(void)
*/
/*----------------------------------------------------------------------------*/
void usb_mic_transfer(void);
/*----------------------------------------------------------------------------*/
/**@brief   USB 端点中断服务程序
   @param   void
   @return  void
   @note    void USB_isr(void)
*/
/*----------------------------------------------------------------------------*/
void usbs_ctl_isr(void);


u8 MCU_SRAMToUSB(void *pBuf, u16 uCount);
u8 MCU_USBToSRAM(void *pBuf, u16 uCount, sUSB_SLAVE_MASS *mass_var);
u8 GetCBW(sUSB_SLAVE_MASS *mass_var);
u32 usb_read_device(u16 lba_num, u32 lba, sUSB_SLAVE_MASS *mass_var);
u32 usb_write_device(u16 lba_num, u32 lba, sUSB_SLAVE_MASS *mass_var);
void usb_device_register(sUSB_DEV_IO *ptr);
u32 usb_write_dev_null(void *buf, u32 lba);
u32 usb_read_dev_null(void *buf, u32 lba);
sUSB_DEV_IO *get_usb_device_registry_ptr(u8 index);
u8 usb_dev_init_null(void);
u32 usb_get_dev_capacity_null(void);
u8 usb_wiat_sd_end_null(u8 op_sd, u8 retry);
void usb_spk_syn_init(void);
void usb_mic_syn_init(void);


/*自定议命令*/
//一级命令
#define UPDATA_WRITE_FLASH    			0xFB
#define UPDATA_OTHER_CMD0      			0xFC
#define UPDATA_READ_FLASH     			0xFD
#define UPDATA_OTHER_CMD1      			0xFE
//二级命令
#define BLOCK_ERASE    					0x00
#define PAGE_WRITE     					0x01
#define FLASH_CHIP_ERASE     			0x02
#define SECTOR_ERASE    				0x03
#define WRITE_RAM    					0x04   	//+
#define WRITE_OTP   					0x05
#define VERIFY_OTP   					0x06
#define WRITE_FPGA_RAM  				0x07	//+
#define READ_FPGA_RAM   				0x08	//+
#define SET_VARIABLE_VALUE  			0x09
#define WRITE_FPGA_DISPERSER_RAM  		0x0b
#define READ_FPGA_DISPERSER_RAM  		0x0a
#define READ_FPGA_DISPERSER_RAM_SENDADDR 0x0c
#define READ_ID        					0x00
#define PAGE_READ      					0x01

#define CHECK_FLASH    					0x00
#define READ_STATUS    					0x01
//#define RUN_APP        					0x02	//+
#define GET_VERSION    					0x03	//+
#define WRITE_OTP_START  				0x04
#define ISD_START  						0x05
#define VERIFY_RESULT  					0x06
#define GET_WRITE_RESULT   				0x07
#define SET_KEY							0x08
#define CHANGE_JTAG_PORT  				0x09  // other cmd
#define READ_KEY                        0x0a
#define GET_ONLINE_DEVICE		        0x0b
//#define SET_FLASH_CMD    				0x09
/////////////////NEW CMD
//+ READ_RAM
//+ 6 FLASH CMD
//+


#endif	/*	_USB_SLAVE_SCSI_H_	*/
